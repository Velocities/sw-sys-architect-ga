#!/usr/bin/env bash
set -euo pipefail

# Builds/starts the dev container on this machine and exposes it on
# 127.0.0.1:2222 so Cursor's Remote-SSH can open it as a remote host.
#
# Cursor cannot drive the Dev Containers extension through an SSH remote, so
# the container is reached as its own SSH target instead.

cd "$(dirname "$0")/.."

SSH_PORT=2222

if ! command -v devcontainer >/dev/null 2>&1; then
    echo "The devcontainer CLI is required:" >&2
    echo "  npm install -g @devcontainers/cli" >&2
    exit 1
fi

UP_OUTPUT="$(devcontainer up --workspace-folder . | tee /dev/stderr)"
CONTAINER_ID="$(printf '%s' "${UP_OUTPUT}" | grep -o '"containerId":"[^"]*"' | tail -1 | cut -d'"' -f4)"

if [[ -z "${CONTAINER_ID}" ]]; then
    echo "Could not determine the container id." >&2
    exit 1
fi

# The container already trusts every key in this machine's authorized_keys.
# Also trust this machine's own public keys so it can be tested from here.
shopt -s nullglob
for pub in "${HOME}"/.ssh/*.pub; do
    docker exec -i -u vscode "${CONTAINER_ID}" \
        bash -c 'cat >> ~/.ssh/authorized_keys' < "${pub}"
done
shopt -u nullglob

docker exec -u vscode "${CONTAINER_ID}" bash -c \
    'sort -u ~/.ssh/authorized_keys -o ~/.ssh/authorized_keys && chmod 600 ~/.ssh/authorized_keys'

echo ""
echo "Container ${CONTAINER_ID:0:12} is up. Verifying SSH on 127.0.0.1:${SSH_PORT}..."

if ssh -o StrictHostKeyChecking=no \
       -o UserKnownHostsFile=/dev/null \
       -o LogLevel=ERROR \
       -o ConnectTimeout=10 \
       -p "${SSH_PORT}" vscode@127.0.0.1 \
       'echo "  SSH OK as $(whoami) in $(ls -d /workspaces/*)"'; then
    echo ""
    echo "Ready. From Cursor on your workstation, connect to the SSH host 'evoarch-container'"
    echo "and open the folder /workspaces/sw-sys-architect-ga"
else
    echo "SSH verification failed. Inspect the container directly with:" >&2
    echo "  docker exec -it -u vscode ${CONTAINER_ID:0:12} bash" >&2
    exit 1
fi
