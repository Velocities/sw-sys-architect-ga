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