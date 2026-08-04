# Deliberately not based on mcr.microsoft.com/devcontainers/cpp: that image's
# devcontainer.metadata label requests ms-vscode.cpptools, which is absent from
# Cursor's marketplace and aborts the remote extension host handshake.
FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    ca-certificates \
    ccache \
    clang-format \
    clang-tidy \
    cmake \
    curl \
    doxygen \
    gdb \
    git \
    graphviz \
    less \
    lldb \
    locales \
    ninja-build \
    openssh-server \
    pkg-config \
    procps \
    python3 \
    sudo \
    tar \
    unzip \
    valgrind \
    wget \
    zip \
    && locale-gen en_US.UTF-8 \
    && rm -rf /var/lib/apt/lists/*

ENV LANG=en_US.UTF-8

# uid/gid 1000 matches the host user so bind-mounted files stay writable.
ARG USERNAME=vscode
ARG USER_UID=1000
ARG USER_GID=1000

RUN if id -u ubuntu >/dev/null 2>&1; then userdel -r ubuntu; fi \
    && groupadd --gid ${USER_GID} ${USERNAME} \
    && useradd --uid ${USER_UID} --gid ${USER_GID} --create-home --shell /bin/bash ${USERNAME} \
    && echo "${USERNAME} ALL=(root) NOPASSWD:ALL" > /etc/sudoers.d/${USERNAME} \
    && chmod 0440 /etc/sudoers.d/${USERNAME}

# Cursor on Windows cannot drive Dev Containers through an SSH remote, so the
# container is reachable as its own SSH target instead. Keys only, no passwords.
RUN mkdir -p /run/sshd \
    && printf '%s\n' \
        'Port 22' \
        'PermitRootLogin no' \
        'PasswordAuthentication no' \
        'ChallengeResponseAuthentication no' \
        'PubkeyAuthentication yes' \
        'AuthorizedKeysFile .ssh/authorized_keys' \
        'AllowUsers vscode' \
        'AcceptEnv LANG LC_*' \
        'ClientAliveInterval 30' \
        'ClientAliveCountMax 10' \
        > /etc/ssh/sshd_config.d/devcontainer.conf

ENV CCACHE_DIR=/home/${USERNAME}/.ccache
ENV PATH="/usr/lib/ccache:${PATH}"

RUN git clone --depth 1 https://github.com/microsoft/vcpkg.git /opt/vcpkg \
    && /opt/vcpkg/bootstrap-vcpkg.sh -disableMetrics \
    && chown -R ${USER_UID}:${USER_GID} /opt/vcpkg

ENV VCPKG_ROOT=/opt/vcpkg
ENV PATH="${VCPKG_ROOT}:${PATH}"

USER ${USERNAME}
