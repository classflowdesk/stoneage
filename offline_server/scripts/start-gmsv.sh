#!/bin/bash
set -euo pipefail

until nc -z saac 9300; do
    sleep 1
done

mkdir -p /state/gmsv-log
cp /opt/stoneage/gmsv/log.cf /state/gmsv-log/log.cf

cd /opt/stoneage/gmsv
exec ./gmsvjt.exe
