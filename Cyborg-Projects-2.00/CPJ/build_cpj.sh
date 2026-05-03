#!/usr/bin/env bash

# Root of FIRE-GEM project
ROOT="/c/Apache24/htdocs/FIRE-GEM/LLM/fire-gem-2.0"

CPJ="$ROOT/CLASS/CPJ"
BOX="$ROOT/CLASS/BOX"
OUT="$CPJ/bin"

mkdir -p "$OUT"

echo "[CPJ] Building scanner (scanner.c + scanner_ops.c + root.c + BOX logs)..."

gcc -O2 -static \
    "$CPJ/scanner.c" \
    "$CPJ/scanner_ops.c" \
    "$CPJ/root.c" \
    "$BOX/src/BOX_DEBUG.c" \
    -I"$CPJ" \
    -I"$BOX/include" \
    -Wl,-subsystem,windows \
    -o "$OUT/FFF-CPJE.exe" \
    -luser32 -lkernel32 -lgdi32 -lshlwapi


echo "[CPJ] Build complete: $OUT/CPJE.exe"
read -p "Press ENTER to exit..." dummy
