#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
GAME="$ROOT/lua_game"
CORE="$GAME/tilengine_libretro.dylib"

if [[ ! -f "$CORE" ]]; then
	CORE="$ROOT/src/tilengine_libretro.dylib"
fi

if [[ ! -f "$CORE" ]]; then
	echo "Core not found. Build it first: make -C src" >&2
	exit 1
fi

cd "$GAME"
exec /Applications/RetroArch.app/Contents/MacOS/RetroArch -L "$CORE" "$GAME" -v
