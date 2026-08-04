#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")/.."

if [[ -f build/dev/CMakeCache.txt ]] && grep -q "/workspaces/evoarch" build/dev/CMakeCache.txt; then
    echo "Removing stale CMake cache from a previous mount path..."
    rm -rf build/dev
fi

cmake --preset dev
cmake --build --preset dev
ctest --preset dev

echo ""
echo "Ready. Run the demo with:"
echo "  ./build/dev/architecture_demo"
