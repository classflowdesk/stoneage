#!/bin/bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
SERVER_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
IMAGE="stoneage-offline-server:2.5"
DB_IMAGE="mariadb@sha256:445bb26f9e3228157113d2be32b71675c7567a193e8272641d2607aa6373bb0b"
NETWORK="stoneage-offline"
DB_CONTAINER="stoneage-db"
SAAC_CONTAINER="stoneage-saac"
GMSV_CONTAINER="stoneage-gmsv"
GMSV_HOST_PORT="19065"
DB_VOLUME="stoneage_db"
STATE_VOLUME="stoneage_state"

log()
{
    printf '[StoneAge] %s\n' "$*"
}

fail()
{
    log "$*" >&2
    docker logs --tail 80 "$SAAC_CONTAINER" 2>/dev/null || true
    docker logs --tail 80 "$GMSV_CONTAINER" 2>/dev/null || true
    exit 1
}

wait_for()
{
    local description="$1"
    local attempts="$2"
    shift 2

    while [ "$attempts" -gt 0 ]; do
        if "$@" >/dev/null 2>&1; then
            return 0
        fi
        attempts=$((attempts - 1))
        sleep 1
    done

    fail "Timed out waiting for $description."
}

command -v docker >/dev/null 2>&1 || fail "Docker is not installed."
docker info >/dev/null 2>&1 || fail "Docker is not running."

if ! docker image inspect "$IMAGE" >/dev/null 2>&1; then
    log "Building the offline server image (first launch only)..."
    docker build --platform linux/amd64 -t "$IMAGE" "$SERVER_DIR"
fi

docker network inspect "$NETWORK" >/dev/null 2>&1 \
    || docker network create "$NETWORK" >/dev/null
docker volume create "$DB_VOLUME" >/dev/null
docker volume create "$STATE_VOLUME" >/dev/null

if ! docker container inspect "$DB_CONTAINER" >/dev/null 2>&1; then
    log "Creating the local database..."
    docker run -d \
        --name "$DB_CONTAINER" \
        --platform linux/arm64 \
        --network "$NETWORK" \
        --network-alias db \
        -e MARIADB_ROOT_PASSWORD=stoneage \
        -e MARIADB_DATABASE=sa \
        -v "$DB_VOLUME:/var/lib/mysql" \
        -v "$SERVER_DIR/db/init.sql:/docker-entrypoint-initdb.d/01-stoneage.sql:ro" \
        "$DB_IMAGE" >/dev/null
else
    docker start "$DB_CONTAINER" >/dev/null
fi

wait_for "the database" 90 \
    docker exec "$DB_CONTAINER" mariadb -uroot -pstoneage -e "SELECT 1"
docker exec -i "$DB_CONTAINER" mariadb -uroot -pstoneage \
    <"$SERVER_DIR/db/init.sql"

docker rm -f "$SAAC_CONTAINER" "$GMSV_CONTAINER" >/dev/null 2>&1 || true

log "Starting account and game services..."
docker run -d \
    --name "$SAAC_CONTAINER" \
    --platform linux/amd64 \
    --network "$NETWORK" \
    --network-alias saac \
    -v "$STATE_VOLUME:/state" \
    "$IMAGE" /usr/local/bin/start-saac >/dev/null

wait_for "the account service" 60 \
    docker exec "$SAAC_CONTAINER" nc -z 127.0.0.1 9300

docker run -d \
    --name "$GMSV_CONTAINER" \
    --platform linux/amd64 \
    --network "$NETWORK" \
    -p "127.0.0.1:$GMSV_HOST_PORT:9065" \
    -v "$STATE_VOLUME:/state" \
    -v "$SERVER_DIR/config/setup.cf:/opt/stoneage/gmsv/setup.cf:ro" \
    -v "$SERVER_DIR/scripts/start-gmsv.sh:/mnt/start-gmsv.sh:ro" \
    "$IMAGE" /bin/bash /mnt/start-gmsv.sh >/dev/null

wait_for "the game service" 120 \
    docker exec "$GMSV_CONTAINER" awk \
    '$2 ~ /:2369$/ && $4 == "0A" { found=1 } END { exit(found ? 0 : 1) }' \
    /proc/net/tcp
sleep 15
docker inspect -f '{{.State.Running}}' "$GMSV_CONTAINER" | grep -q true \
    || fail "The game service exited during initialization."
docker exec "$GMSV_CONTAINER" awk \
    '$2 ~ /:2369$/ && $4 == "0A" { found=1 } END { exit(found ? 0 : 1) }' \
    /proc/net/tcp \
    || fail "The game service stopped accepting connections."
log "Offline server is ready at 127.0.0.1:$GMSV_HOST_PORT."
