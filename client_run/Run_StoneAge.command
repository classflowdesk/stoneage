#!/bin/bash

set -u

PROJECT_DIR="/Users/xiangli/xli/AI/projects/StoneAge"
SERVER_START="$PROJECT_DIR/offline_server/scripts/start-local.sh"
SERVER_LIST="$PROJECT_DIR/client_lua/list.lua"
WINE_ROOT="/Applications/Wine Stable.app/Contents/Resources/wine"
WINE="$WINE_ROOT/bin/wine"
WINE_PREFIX="$HOME/Library/Application Support/StoneAgeWine"
GAME_DIR="$HOME/Library/Containers/com.isaacmarovitz.Whisky/Bottles/C8D9667E-FE13-433E-B76E-9B6EA3E75D9A/drive_c/stoneage"
GAME_EXE="$GAME_DIR/stoneage.exe"
DDRAW_DLL="$GAME_DIR/ddraw.dll"
DDRAW_INI="$GAME_DIR/ddraw.ini"
AUTO_DATA="$GAME_DIR/data/auto.dat"
LOG_DIR="$HOME/Library/Logs/StoneAge"
LOG_FILE="$LOG_DIR/launcher.log"

mkdir -p "$LOG_DIR"

fail()
{
    echo
    echo "StoneAge failed to start: $1"
    echo "Log: $LOG_FILE"
    echo
    read -r -p "Press Return to close..."
    exit 1
}

verify_size()
{
    local path="$1"
    local minimum="$2"
    local label="$3"
    local size

    [ -f "$path" ] || fail "missing $label: $path"
    size="$(stat -f '%z' "$path" 2>/dev/null || echo 0)"
    [ "$size" -ge "$minimum" ] || fail "$label is incomplete: $path"
}

[ -x "$WINE" ] || fail "Wine Stable is not installed at $WINE"
[ -x "$SERVER_START" ] || fail "offline server launcher is missing: $SERVER_START"
[ -f "$SERVER_LIST" ] || fail "offline server list is missing: $SERVER_LIST"
[ -d "$WINE_PREFIX" ] || fail "Wine prefix is missing: $WINE_PREFIX"
[ -d "$GAME_DIR" ] || fail "game directory is missing: $GAME_DIR"

verify_size "$GAME_EXE" 1000000 "source-built StoneAge executable"
verify_size "$GAME_DIR/data/real_31.bin" 1000000000 "real_31.bin"
verify_size "$GAME_DIR/data/adrn_31.bin" 20000000 "adrn_31.bin"
verify_size "$GAME_DIR/data/spr_17.bin" 6000000 "spr_17.bin"
verify_size "$GAME_DIR/data/spradrn_17.bin" 10000 "spradrn_17.bin"
verify_size "$GAME_DIR/data/pal/PALET_1.SAP" 600 "palette data"
verify_size "$DDRAW_DLL" 400000 "cnc-ddraw runtime"
verify_size "$DDRAW_INI" 1000 "cnc-ddraw configuration"

grep -Eiq '^[[:space:]]*renderer[[:space:]]*=[[:space:]]*gdi[[:space:]]*$' "$DDRAW_INI" \
    || fail "ddraw.ini must use the GDI renderer"

if [ -f "$AUTO_DATA" ]; then
    auto_size="$(stat -f '%z' "$AUTO_DATA" 2>/dev/null || echo 0)"
    if [ "$auto_size" -ne 1000010 ]; then
        rm -f "$AUTO_DATA" || fail "could not repair data/auto.dat"
    fi
fi

{
    echo
    echo "=== $(date '+%Y-%m-%d %H:%M:%S') Starting offline StoneAge ==="
    "$SERVER_START"
} >>"$LOG_FILE" 2>&1 || fail "the offline server did not become ready"

cp "$SERVER_LIST" "$GAME_DIR/lua/list.lua" \
    || fail "could not install the offline server list"

if pgrep -f '[s]toneage.exe OpenClient' >/dev/null 2>&1; then
    echo "StoneAge is already running."
    exit 0
fi

cd "$GAME_DIR" || fail "could not enter the game directory"
export PATH="$WINE_ROOT/bin:$PATH"
export WINEPREFIX="$WINE_PREFIX"
export WINEDEBUG="-all"
export WINEDLLOVERRIDES="ddraw=n,b"
export MVK_CONFIG_LOG_LEVEL="0"

echo "Offline server is ready."
echo "Login: stoneage / stoneage"
echo "Keep this Terminal window open while playing."
echo "Log: $LOG_FILE"

exec "$WINE" "$GAME_EXE" \
    OpenClient \
    windowmode \
    realbin:31 \
    adrnbin:31 \
    sprbin:17 \
    spradrnbin:17 \
    >>"$LOG_FILE" 2>&1
