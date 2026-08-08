#!/bin/bash
set -e

echo "=== StoneAge Mac Development Build & Runtime Verification ==="

EXE_PATH="client/build/VER25_RELEASE/stoneage.exe"

if [ -f "$EXE_PATH" ]; then
    echo "[✓] Found compiled executable: $EXE_PATH"
    ls -lh "$EXE_PATH"
else
    echo "[!] Executable not found at $EXE_PATH"
fi
