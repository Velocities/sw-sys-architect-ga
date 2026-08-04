#!/usr/bin/env bash
set -euo pipefail

# Authorize whoever can already reach the Docker host, so no new keys are needed.
HOST_KEYS="/tmp/host-authorized-keys"

if [[ -f "${HOST_KEYS}" ]]; then
    mkdir -p "${HOME}/.ssh"
    install -m 600 "${HOST_KEYS}" "${HOME}/.ssh/authorized_keys"
    chmod 700 "${HOME}/.ssh"
else
    echo "WARNING: ${HOST_KEYS} not mounted; SSH logins will be refused." >&2
fi

sudo mkdir -p /run/sshd
sudo ssh-keygen -A

if pgrep -x sshd >/dev/null 2>&1; then
    echo "sshd already running."
else
    sudo /usr/sbin/sshd
    echo "sshd started on container port 22."
fi
