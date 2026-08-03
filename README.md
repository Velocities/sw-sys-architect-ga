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
3. Wait for the container image to build on first launch. This can take several minutes while vcpkg downloads and compiles dependencies.
4. On first open, the container automatically runs configure, build, and tests via `postCreateCommand`.

After setup completes, you should have a working build in `build/dev/` and passing smoke tests.

### Rebuilding the Container

Rebuild the container if you change `.devcontainer/Dockerfile` or need a clean environment:

- Command Palette → **Dev Containers: Rebuild Container**

---

## Common Commands

All commands below assume you are inside the dev container and at the project root.

### Build and Test

| Task | Command |
|------|---------|
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