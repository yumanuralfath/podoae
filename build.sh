#!/bin/bash
set -e

CC="gcc"
VERSION=$(cat VERSION 2>/dev/null || echo "1.0.0")
CFLAGS=(-Wall -Wextra -IInclude -DPODOAE_VERSION="\"$VERSION\"")
LDFLAGS=(-lSDL2 -lGL) # Tambahkan flag library SDL2 dan OpenGL

# Pastikan file renderer.c di-include saat build
SRC=(main.c podoae.c Include/microui/microui.c Include/renderer/renderer.c)
OUT="podoae"

echo "Izin Compile dulu bang..."
"$CC" "${CFLAGS[@]}" "${SRC[@]}" "${LDFLAGS[@]}" -o "$OUT"
echo "Alhamdulillah selesai. Jalankan ./$OUT"
