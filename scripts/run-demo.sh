#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")/.."

if [[ ! -x build/dev/architecture_demo ]]; then
    echo "Demo not built yet. Running dev-setup first..."
    bash scripts/dev-setup.sh
fi

./build/dev/architecture_demo
