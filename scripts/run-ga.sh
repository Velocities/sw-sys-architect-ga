#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")/.."

if [[ ! -x build/dev/ga_run ]]; then
    echo "GA runner not built yet. Running dev-setup first..."
    bash scripts/dev-setup.sh
fi

GA_CONFIG="${1:-configs/example_ga.json}"
WORKLOAD="${2:-configs/example_workload.json}"
SIMULATOR="${3:-configs/example_simulator.json}"

./build/dev/ga_run "$GA_CONFIG" "$WORKLOAD" "$SIMULATOR"
