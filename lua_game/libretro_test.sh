#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
GAME="$ROOT/lua_game"

case "$(uname -s)" in
    Darwin)
        CORE="$GAME/tilengine_libretro.dylib"
        [[ -f "$CORE" ]] || CORE="$ROOT/src/tilengine_libretro.dylib"

        RETROARCH=(
            /Applications/RetroArch.app/Contents/MacOS/RetroArch
        )
        ;;

    Linux)
        CORE="$GAME/tilengine_libretro.so"
        [[ -f "$CORE" ]] || CORE="$ROOT/src/tilengine_libretro.so"

        RETROARCH=(
            flatpak run org.libretro.RetroArch
        )

        flatpak override --user \
            --filesystem="$ROOT" \
            org.libretro.RetroArch
        ;;

    *)
        echo "Unsupported platform: $(uname -s)" >&2
        exit 1
        ;;
esac

if [[ ! -f "$CORE" ]]; then
    echo "Core not found: $CORE" >&2
    echo "Build it first: make -C src" >&2
    exit 1
fi

cd "$GAME"

exec "${RETROARCH[@]}" -L "$CORE" "$GAME" -v
