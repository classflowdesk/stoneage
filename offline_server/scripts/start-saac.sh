#!/bin/bash
set -euo pipefail

until mariadb-admin ping -h db -uroot -pstoneage --silent; do
    sleep 1
done

cd /opt/stoneage/saac
exec ./saacjt.exe
