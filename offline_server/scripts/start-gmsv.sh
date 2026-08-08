#!/bin/bash
set -euo pipefail

until nc -z saac 9300; do
    sleep 1
done

cd /opt/stoneage/gmsv
exec ./gmsvjt.exe
