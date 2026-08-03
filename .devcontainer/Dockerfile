FROM mcr.microsoft.com/devcontainers/cpp:ubuntu-24.04

# Install development tools
RUN apt-get update && apt-get install -y \
    ninja-build \
    ccache \
    graphviz \
    doxygen \
    valgrind \
    clang-tidy \
    clang-format \
    gdb \
    lldb \
    pkg-config \
    zip \
    unzip \
    curl \
    git \
    && rm -rf /var/lib/apt/lists/*

# Enable ccache by default
ENV CCACHE_DIR=/home/vscode/.ccache
ENV PATH="/usr/lib/ccache:${PATH}"

# Install vcpkg
RUN git clone https://github.com/microsoft/vcpkg.git /opt/vcpkg && \
    /opt/vcpkg/bootstrap-vcpkg.sh

ENV VCPKG_ROOT=/opt/vcpkg
ENV PATH="${VCPKG_ROOT}:${PATH}"