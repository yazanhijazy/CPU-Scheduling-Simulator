# CPU Scheduling Simulator

A C++17 **Operating Systems** coursework project that simulates and compares three classic CPU scheduling algorithms:

- **FCFS** — First-Come, First-Served
- **SJF** — Shortest Job First, non-preemptive
- **Round Robin** — preemptive, with a configurable time quantum

The simulator prints an execution trace and calculates completion, turnaround, and waiting-time metrics for every process.

## Features

- FCFS scheduling
- Non-preemptive SJF scheduling
- Round Robin scheduling
- CPU idle-gap handling
- Deterministic tie-breaking
- Input validation
- Per-process scheduling metrics
- Average turnaround time
- Average waiting time
- Automated tests for important scheduling edge cases
- Make and CMake build support

## Scheduling Rules

### FCFS

Processes are ordered by arrival time. If two processes arrive together, the smaller PID is selected first.

### SJF

Among processes that have already arrived, the process with the shortest burst time is selected.

Tie-breaking order:

1. shorter burst time
2. earlier arrival time
3. smaller PID

### Round Robin

Processes execute for at most one time quantum at a time.

A coursework-specific boundary rule is preserved:

> If a new process arrives exactly when a time quantum expires, the new arrival is inserted into the ready queue **before** the preempted process is re-queued.

## Input Format

Each non-comment line contains:

```text
PID ArrivalTime BurstTime
```

Example:

```text
1 0 24
2 1 3
3 2 3
```

Constraints:

- PID must be positive and unique.
- Arrival time must be non-negative.
- Burst time must be positive.
- Time quantum must be a positive integer.
- Blank lines and lines beginning with `#` are ignored.

## Build and Run

### Make

```bash
make
./scheduler examples/tasks.txt 4
```

Run the automated tests with:

```bash
make test
```

### CMake

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
./build/scheduler examples/tasks.txt 4
```

On Windows with a multi-configuration CMake generator, the executable may be inside `build/Debug/` or `build/Release/`.

## Metrics

For each process the simulator reports:

| Metric | Meaning |
|---|---|
| `PID` | Process ID |
| `AT` | Arrival Time |
| `BT` | Burst Time |
| `CT` | Completion Time |
| `TT` | Turnaround Time = `CT - AT` |
| `WT` | Waiting Time = `TT - BT` |

## Coursework Example

Input:

```text
1 0 24
2 1 3
3 2 3
```

With Round Robin quantum `4`, the completion times are:

| PID | FCFS CT | SJF CT | RR CT |
|---:|---:|---:|---:|
| 1 | 24 | 24 | 30 |
| 2 | 27 | 27 | 7 |
| 3 | 30 | 30 | 10 |

For this input, Round Robin reduces the average waiting time from `16.00` to approximately `4.67`.

## Repository Structure

```text
CPU-Scheduling-Simulator/
├── .github/
│   └── workflows/
│       └── build.yml
├── examples/
│   └── tasks.txt
├── include/
│   └── scheduler.hpp
├── src/
│   ├── main.cpp
│   └── scheduler.cpp
├── tests/
│   └── test_scheduler.cpp
├── CMakeLists.txt
├── Makefile
├── .gitattributes
├── .gitignore
└── README.md
```

## Technologies and Concepts

- C++17
- Operating Systems
- CPU Scheduling
- FCFS
- SJF
- Round Robin
- Ready Queues
- Priority Queues
- Process Metrics
- Automated Testing
