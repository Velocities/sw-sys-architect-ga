# EvoArch

**EvoArch** is a system architecture simulator and optimization framework.

The goal of the project is to model distributed systems using graph-based representations, simulate realistic workloads, and evaluate architectures using configurable performance metrics. Future versions will use genetic algorithms to automatically evolve architectures that optimize for latency, throughput, cost, reliability, and other objectives.

---

## Goals

- Model distributed systems as directed graphs
- Simulate realistic request workflows
- Measure architecture performance
- Inject failures and evaluate resilience
- Optimize architectures using genetic algorithms

---

## Planned Features

### Architecture Modeling

- APIs
- Load balancers
- Databases
- Database replicas
- Redis
- Message queues
- Worker services
- Custom services

---

### Simulation

- Discrete-event simulation
- Configurable workloads
- Network latency
- Processing latency
- Request queues
- Cache hits/misses
- Replication delays
- Failure injection

---

### Metrics

- Average latency
- P95 / P99 latency
- Throughput
- Availability
- Infrastructure cost
- Resource utilization
- Reliability score

---

### Optimization

- Genetic algorithms
- Multi-objective optimization
- Architecture mutation
- Architecture crossover
- Pareto frontier generation

---

## Project Structure

```
include/
    architecture/
    simulation/
    services/
    ga/

src/

configs/

tests/
```

---

## Tech Stack

EvoArch is built using a modern C++ development environment focused on portability, reproducibility, and maintainability.

### Core Language

**C++23**

The simulator, graph engine, and optimization algorithms are written in modern C++ for performance and fine-grained control over memory and data structures.

---

### Build System

**CMake**

CMake generates platform-independent build files and manages project configuration. It is responsible for locating libraries, compiling source files, and organizing the project into targets.

---

### Build Tool

**Ninja**

Ninja is a fast build system used by CMake. It performs incremental builds efficiently, rebuilding only what has changed.

---

### Dependency Management

**vcpkg**

vcpkg installs and manages third-party C++ libraries. Instead of manually downloading dependencies, the project specifies required packages and vcpkg installs the correct versions automatically.

Current dependencies include:

- Boost Graph Library
- nlohmann/json
- GoogleTest

---

### Graph Library

**Boost Graph Library (BGL)**

The simulator models distributed systems as directed graphs. Boost Graph Library provides graph data structures and algorithms that form the foundation of the architecture model.

---

### JSON Parsing

**nlohmann/json**

Architecture definitions, workloads, and simulator configuration are stored as JSON files and parsed using nlohmann/json.

---

### Unit Testing

**GoogleTest**

GoogleTest provides the unit testing framework used to verify simulator behavior, graph operations, and future genetic algorithm implementations.

---

### Compiler Cache

**ccache**

ccache caches compiled object files to dramatically reduce rebuild times after small source code changes.

---

### Formatting

**clang-format**

Automatically formats C++ code using a shared style so the entire project maintains consistent formatting.

---

### Static Analysis

**clang-tidy**

Performs static analysis on the source code to identify potential bugs, unsafe patterns, and opportunities to modernize the codebase.

---

### Documentation

**Doxygen**

Generates API documentation directly from comments within the source code.

---

### Development Environment

**Docker Dev Containers**

The project uses a Docker-based development environment.

Instead of requiring each contributor to install compilers and libraries manually, the development container contains the complete toolchain, including:

- C++ compiler
- CMake
- Ninja
- vcpkg
- Boost
- GoogleTest
- Graphviz
- clang-format
- clang-tidy
- Doxygen
- ccache

Opening the project inside the development container provides an identical development environment on any machine with Docker installed.

This ensures builds are reproducible and eliminates "works on my machine" issues.

---

## Getting Started

### Prerequisites

To use the recommended development workflow, you need:

- [Docker](https://docs.docker.com/get-docker/) installed and running
- [VS Code](https://code.visualstudio.com/) or [Cursor](https://cursor.com/) with the [Dev Containers extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers)

All compilers, libraries, and tooling are provided inside the dev container — nothing else needs to be installed on your host machine.

### Opening the Dev Container

1. Clone the repository and open it in VS Code or Cursor.
2. When prompted, click **Reopen in Container**.  
   If you are not prompted, open the Command Palette (`Ctrl+Shift+P` / `Cmd+Shift+P`) and run **Dev Containers: Reopen in Container**.
3. Wait for the container image to build on first launch. This takes a few minutes.
4. Once attached, run the one-time setup from a terminal inside the container:

```bash
./scripts/dev-setup.sh
```

This configures CMake, installs vcpkg dependencies, builds, and runs the tests. Afterwards you have a working build in `build/dev/` and passing smoke tests.

To confirm the architecture model works end to end:

```bash
./scripts/run-demo.sh
```

Setup is intentionally *not* wired to `postCreateCommand`, so attaching to the container is fast and never blocked by a long build.

### Opening the Container When Docker Runs on a Remote Machine

If you run Cursor on a workstation but keep the repository and Docker on a separate Linux box, **Reopen in Container will not work**. Cursor cannot drive the Dev Containers extension through an SSH remote — every attempt fails with `WebSocket close with status code 1006`.

The container therefore doubles as its own SSH target. On the Linux machine that runs Docker:

```bash
npm install -g @devcontainers/cli   # once
./scripts/container-up.sh
```

That builds and starts the container, starts `sshd` inside it, and publishes it on `127.0.0.1:2222` of the Docker host. The container trusts every key already listed in that machine's `~/.ssh/authorized_keys`, so no new keys are needed.

Then add this to `~/.ssh/config` on your workstation, replacing `my-linux-box` with the SSH host you already use to reach the Docker machine:

```sshconfig
Host evoarch-container
    HostName 127.0.0.1
    Port 2222
    User vscode
    ProxyJump my-linux-box
    StrictHostKeyChecking no
    UserKnownHostsFile /dev/null
```

On Windows use `UserKnownHostsFile NUL` instead of `/dev/null`.

The port is published only on the Docker host's loopback interface, so `ProxyJump` tunnels in through the machine you already trust and nothing new is exposed to the network. `UserKnownHostsFile` is discarded because the container regenerates its host keys whenever the image is rebuilt.

In Cursor: **Remote-SSH: Connect to Host** → `evoarch-container` → **Open Folder** → `/workspaces/sw-sys-architect-ga`.

Re-run `./scripts/container-up.sh` after a reboot or whenever the container has been stopped; it restarts the container and `sshd` and is safe to run repeatedly.

### Rebuilding the Container

Rebuild the container if you change `.devcontainer/Dockerfile` or need a clean environment:

- Command Palette → **Dev Containers: Rebuild Container**
- Or, when using the SSH workflow above: `devcontainer up --workspace-folder . --remove-existing-container`

### Notes on the Container Image

The image is built from `ubuntu:24.04` rather than `mcr.microsoft.com/devcontainers/cpp`. The Microsoft C++ image advertises `ms-vscode.cpptools` through its `devcontainer.metadata` label, and that extension does not exist in Cursor's marketplace. Cursor fails the extension install during startup and the remote extension host handshake is aborted, which surfaces as `WebSocket close with status code 1006`.

Because of that, IntelliSense is provided by **clangd** (backed by `build/dev/compile_commands.json`) and the cpptools engine is explicitly disabled. Run `./scripts/dev-setup.sh` at least once so clangd has a compilation database to read.

If the container fails to attach, you can always build and run without the IDE:

```bash
devcontainer up --workspace-folder .
docker exec -it -u vscode -w /workspaces/sw-sys-architect-ga <container_id> bash
```

---

## Common Commands

All commands below assume you are inside the dev container and at the project root.

### Build and Test

| Task | Command |
|------|---------|
| Start the container + SSH (run on the Docker host) | `./scripts/container-up.sh` |
| First-time setup (configure + build + test) | `./scripts/dev-setup.sh` |
| Run the architecture demo | `./scripts/run-demo.sh` |
| Configure (Debug) | `cmake --preset dev` |
| Build (Debug) | `cmake --build --preset dev` |
| Run tests | `ctest --preset dev` |
| Configure + build + test | `cmake --preset dev && cmake --build --preset dev && ctest --preset dev` |
| Release build | `cmake --preset release && cmake --build --preset release` |
| Build with clang-tidy | `cmake --preset tidy && cmake --build --preset tidy` |

### Code Quality

| Task | Command |
|------|---------|
| Format all source | `clang-format -i $(find include src tests -name '*.cpp' -o -name '*.hpp')` |
| Generate API docs | `cmake --build --preset dev --target docs` |

Generated documentation is written to `docs/generated/html/`. Open `docs/generated/html/index.html` in a browser to view it.

### CMake Presets

The project uses [CMake Presets](https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html) defined in `CMakePresets.json`:

| Preset | Purpose |
|--------|---------|
| `dev` | Debug build with tests enabled (default) |
| `release` | Optimized release build |
| `tidy` | Debug build with clang-tidy enabled during compilation |

IDE integration is preconfigured in `.vscode/settings.json` to use the `dev` preset automatically.

### Configuration Files

Example JSON configs live in `configs/`:

- `example_architecture.json` — graph-based architecture definitions
- `example_workload.json` — request traffic patterns
- `example_simulator.json` — simulator settings and metrics

These are placeholders for now and will be consumed by the simulator as implementation progresses.

### Project Layout (Build Artifacts)

```
build/
    dev/              # Debug build output (default)
    release/          # Release build output
    tidy/             # clang-tidy build output
docs/
    generated/        # Doxygen output (after running docs target)
vcpkg_installed/      # vcpkg dependencies (created during configure)
```

`compile_commands.json` is generated inside `build/dev/` and is used by clangd for IDE IntelliSense.

---

## Current Status

🚧 Early development

The initial milestone is to build the simulation engine capable of executing request workflows over graph-based architectures.

Once the simulator is complete, genetic algorithm support will be added to automatically explore and optimize architecture designs.

---

## Long-Term Vision

EvoArch aims to become a flexible experimentation platform for distributed systems.

Instead of manually designing architectures, users will be able to describe workloads and optimization goals, allowing the system to automatically search for architectures that best satisfy those objectives.

Potential optimization targets include:

- Performance
- Scalability
- Fault tolerance
- Infrastructure cost
- Energy efficiency
- Resource utilization

The project is intended as both a learning platform for distributed systems and evolutionary computing, and as a foundation for experimenting with automated system architecture design.