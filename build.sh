#!/bin/bash
set -e

CC="gcc"
CFLAGS=(-Wall -Wextra -IInclude)
LDFLAGS=(-lSDL2 -lGL) # Tambahkan flag library SDL2 dan OpenGL

# Pastikan file renderer.c di-include saat build
SRC=(main.c pomodoro.c Include/microui/microui.c Include/renderer/renderer.c)
OUT="podoae"

echo "Izin Compile dulu bang..."
"$CC" "${CFLAGS[@]}" "${SRC[@]}" "${LDFLAGS[@]}" -o "$OUT"
echo "Alhamdulillah selesai. Jalankan ./$OUT"
