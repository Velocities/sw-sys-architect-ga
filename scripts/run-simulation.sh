#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")/.."

if [[ ! -x build/dev/simulation_run ]]; then
    echo "Simulation runner not built yet. Running dev-setup first..."
    bash scripts/dev-setup.sh
fi

ARCHITECTURE="${1:-configs/test_architecture.json}"
WORKLOAD="${2:-configs/example_workload.json}"
SIMULATOR="${3:-configs/example_simulator.json}"

./build/dev/simulation_run "$ARCHITECTURE" "$WORKLOAD" "$SIMULATOR"
