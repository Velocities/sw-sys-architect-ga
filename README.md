# EvoArch

**EvoArch** is a system architecture simulator and optimization framework.

The goal of the project is to model distributed systems using graph-based representations, simulate realistic workloads, and evaluate architectures using configurable performance metrics. A genetic algorithm searches over architecture designs by running the simulator and scoring the resulting metrics.

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

EvoArch is organized by responsibility. Static architecture definitions live separately from runtime simulation state and from the optimization layer.

```
sw-sys-architect-ga/
├── include/                  # Public headers
│   ├── architecture/         # Graph model, components, JSON loading
│   ├── services/             # Per-component-type defaults (cost, processing time)
│   ├── simulation/           # Workloads, DES simulator, metrics, failures
│   └── ga/                   # Genome, fitness, population, genetic algorithm
├── src/                      # Implementations (mirrors include/)
├── apps/                     # Standalone executables
│   ├── architecture_demo/    # Hard-coded architecture smoke test
│   ├── architecture_loader_test/
│   ├── simulation_run/       # Run one simulation from JSON configs
│   └── ga_run/               # Run the genetic algorithm
├── configs/                  # JSON configs for architectures, workloads, simulator, GA
├── scripts/                  # dev-setup, run-demo, run-simulation, run-ga, container-up
└── tests/                    # GoogleTest unit tests
```

### Where code belongs

| Layer | Directory | Responsibility |
|-------|-----------|----------------|
| **Architecture** | `include/architecture/`, `include/services/` | What the system *is*: graph topology, component types, performance models, monthly cost defaults |
| **Simulation** | `include/simulation/` | What *happens* during a run: requests, event queue, failures, metrics |
| **Optimization** | `include/ga/` | How architectures are *searched*: genomes, fitness scoring, evolution |
| **Apps** | `apps/` | Thin entry points that load JSON and invoke the library |
| **Configs** | `configs/` | Declarative inputs — no recompile needed to change a scenario |

Key files to start from:

- `include/architecture/Architecture.hpp` — Boost graph of components and edges
- `include/architecture/ArchitectureLoader.hpp` — load architectures from JSON
- `include/simulation/Simulator.hpp` — discrete-event simulation engine
- `include/simulation/Metrics.hpp` — simulation output (latency, throughput, cost, …)
- `include/ga/GeneticAlgorithm.hpp` — evolution loop over architecture genomes

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
| Load and print an architecture from JSON | `./build/dev/architecture_loader_test` |
| Run a simulation | `./scripts/run-simulation.sh` |
| Run the genetic algorithm | `./scripts/run-ga.sh` |
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

JSON configs live in `configs/`:

| File | Purpose |
|------|---------|
| `test_architecture.json` | Example cache-aside topology (LB → API → Redis → Postgres) |
| `example_workload.json` | Workflow mix (Login, Read Feed, …) with traffic percentages |
| `example_simulator.json` | Duration, arrival rate, seed, failure injection |
| `healthy_simulator.json` | Same as above but with failures disabled |
| `example_ga.json` | GA population, generations, genome bounds, fitness weights |

Example — run a healthy simulation:

```bash
./scripts/run-simulation.sh configs/test_architecture.json configs/example_workload.json configs/healthy_simulator.json
```

Example — run the GA with a failure scenario (tests resilience):

```bash
./scripts/run-ga.sh configs/example_ga.json configs/example_workload.json configs/example_simulator.json
```

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

## How the Simulator Works

The simulator is a **discrete-event simulation (DES)** engine. It models requests moving through an architecture graph under a configurable workload and optional failure scenario, then produces a `Metrics` snapshot.

### Pipeline

```
Architecture + Workload + SimulatorConfig
              ↓
          Simulator::run()
              ↓
           Metrics
```

`Metrics` is intentionally separate from the genetic algorithm — the simulator *measures*; downstream code *evaluates* those measurements.

### Main concepts

| Concept | Class / file | Role |
|---------|--------------|------|
| **Architecture** | `Architecture` | Static graph: components (vertices) and network links (edges with latency/bandwidth) |
| **Component** | `Component` | A node with a type (API, Redis, Postgres, LoadBalancer) and a `PerformanceModel` |
| **PerformanceModel** | `PerformanceModel` | Theoretical service time distribution, concurrency limit, monthly cost |
| **Workload** | `Workload` | Named collection of workflows, each with a traffic percentage |
| **Workflow** | `Workflow` | Ordered hops through component *types* (and `Client` endpoints) — not specific instance IDs |
| **Request** | `Request` | One in-flight unit of work tracking position, time, and accumulated latency |
| **ComponentRuntimeState** | `ComponentRuntimeState` | Per-vertex runtime: queue, in-flight count, healthy flag |
| **FailureScenario** | `FailureScenario` | Timed `node_down` / `node_recovery` events targeting component IDs |
| **Metrics** | `Metrics` | Output: latency percentiles, throughput, availability, cost, utilization |

### Event loop

All simulation activity is driven by a time-ordered priority queue (`EventQueue` in `include/simulation/Event.hpp`):

1. **RequestArrival** — spawn a request, pick a workflow by weight, enter the graph at the first component hop
2. **ProcessingComplete** — a component finishes service; advance the workflow to the next hop or complete the request
3. **FailureInjection** — flip a component's health; queued work resumes on recovery

At each component the simulator:

- Samples processing time from the component's distribution (fixed, normal, or log-normal)
- Enforces `maxConcurrentRequests` — excess requests wait in a queue
- Auto-routes to another healthy instance of the same type when the target is down
- Adds network latency from edge properties (or a default when no direct edge exists)

### Failure scenarios

Failures are **scenario-driven**, not random per-request noise. Configure them in the simulator JSON:

```json
"failure_injection": {
    "enabled": true,
    "preset": "kill_primary_db",
    "events": [
        { "type": "node_down", "target": "db-primary", "at_time_ms": 10000, "duration_ms": 5000 }
    ]
}
```

When `duration_ms` is set on a `node_down` event, a matching `node_recovery` is scheduled automatically.

---

## How the Genetic Algorithm Works

The GA searches over **architecture genomes** — compact encodings of how many API, Redis, and Postgres instances to provision — by repeatedly simulating each candidate and scoring the resulting metrics.

### Pipeline

```
Genome → Architecture → Simulator → Metrics → FitnessFunction → fitness score
         ↑__________________________________________|
                    selection / crossover / mutation
```

### Genome

A `Genome` (`include/ga/Genome.hpp`) holds three genes:

- `apiInstances`
- `redisInstances`
- `postgresInstances`

`Genome::toArchitecture()` expands a genome into a full cache-aside topology:

- One load balancer connects to every API instance
- Every API connects to every Redis instance
- Every Redis connects to every Postgres instance (primary + replicas)

Bounds for each gene are set in `configs/example_ga.json` under `"genome"`.

### Fitness function

`FitnessFunction` (`include/ga/Fitness.hpp`) converts `Metrics` into a single score using weighted, normalized terms:

```
fitness = w_throughput × (throughput / ref_throughput)
        + w_latency    × (ref_latency / average_latency)
        + w_cost       × (ref_cost / monthly_cost)
        + w_availability × availability
```

Weights and reference values are configured in the GA JSON under `"fitness"`. Higher fitness is better. Latency and cost terms reward *lower* values by inverting them against a reference.

The simulator and fitness function stay decoupled — you can change scoring without touching simulation logic, or run simulations without the GA.

### Evolution loop

`GeneticAlgorithm::run()` (`include/ga/GeneticAlgorithm.hpp`):

1. Create a random initial `Population`
2. For each generation:
   - **Evaluate** every individual (run the simulator, compute fitness)
   - **Select** parents via tournament selection
   - **Crossover** genomes (uniform gene swap)
   - **Mutate** instance counts (±1 per gene, with configurable probability)
   - **Elitism** — carry the best individual forward unchanged
3. Return the highest-fitness individual after all generations

Each individual is simulated with a distinct seed derived from the GA seed so runs are reproducible but not identical.

### GA configuration

`configs/example_ga.json` controls population size, generation count, mutation/crossover rates, genome bounds, and fitness weights. The workload and simulator configs are the same files used by `./scripts/run-simulation.sh` — the GA evaluates candidates against the same traffic and failure scenario you choose.

---

## Current Status

Implemented:

- Graph-based architecture model with JSON loading
- Component performance models and type-specific cost defaults
- Workloads, workflows, discrete-event simulation, failure injection
- Metrics collection (latency, throughput, availability, cost, utilization)
- Genetic algorithm with configurable fitness function

Planned next:

- Cache hit/miss modeling in simulation
- Multi-objective optimization / Pareto frontiers (NSGA-II)
- Richer genome (edge tuning, additional component types)
- Workload and failure-scenario JSON presets

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