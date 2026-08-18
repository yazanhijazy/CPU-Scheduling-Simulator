#include "scheduler.hpp"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <queue>
#include <set>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace {

void validateProcesses(const std::vector<Process>& processes) {
    if (processes.empty()) {
        throw std::runtime_error("Input file contains no processes.");
    }

    std::set<int> pids;

    for (const auto& process : processes) {
        if (process.pid <= 0) {
            throw std::runtime_error("PID must be a positive integer.");
        }
        if (process.arrival < 0) {
            throw std::runtime_error("Arrival time must be non-negative.");
        }
        if (process.burst <= 0) {
            throw std::runtime_error("Burst time must be positive.");
        }
        if (!pids.insert(process.pid).second) {
            throw std::runtime_error(
                "Duplicate PID detected: " + std::to_string(process.pid));
        }
    }
}

void finalizeMetrics(std::vector<Process>& processes,
                     double& averageTurnaround,
                     double& averageWaiting) {
    long double turnaroundTotal = 0.0L;
    long double waitingTotal = 0.0L;

    for (auto& process : processes) {
        process.turnaround = process.completion - process.arrival;
        process.waiting = process.turnaround - process.burst;
        turnaroundTotal += process.turnaround;
        waitingTotal += process.waiting;
    }

    averageTurnaround =
        static_cast<double>(turnaroundTotal / processes.size());
    averageWaiting =
        static_cast<double>(waitingTotal / processes.size());

    std::sort(processes.begin(), processes.end(),
              [](const Process& lhs, const Process& rhs) {
                  return lhs.pid < rhs.pid;
              });
}

std::string event(Time time, int pid, const std::string& action) {
    return "[Time " + std::to_string(time) + "] Process " +
           std::to_string(pid) + " " + action;
}

void sortByArrivalThenPid(std::vector<Process>& processes) {
    std::sort(processes.begin(), processes.end(),
              [](const Process& lhs, const Process& rhs) {
                  if (lhs.arrival != rhs.arrival) {
                      return lhs.arrival < rhs.arrival;
                  }
                  return lhs.pid < rhs.pid;
              });
}

}  // namespace

std::vector<Process> readProcesses(const std::string& filename) {
    std::ifstream input(filename);
    if (!input) {
        throw std::runtime_error("Failed to open input file: " + filename);
    }

    std::vector<Process> processes;
    std::string line;
    int lineNumber = 0;

    while (std::getline(input, line)) {
        ++lineNumber;

        const auto firstNonSpace = line.find_first_not_of(" \t\r\n");
        if (firstNonSpace == std::string::npos || line[firstNonSpace] == '#') {
            continue;
        }

        std::istringstream stream(line);
        Process process{};
        std::string extra;

        if (!(stream >> process.pid >> process.arrival >> process.burst) ||
            (stream >> extra)) {
            throw std::runtime_error(
                "Invalid input format at line " + std::to_string(lineNumber) +
                ". Expected: PID ArrivalTime BurstTime");
        }

        process.remaining = process.burst;
        processes.push_back(process);
    }

    validateProcesses(processes);
    return processes;
}

Result simulateFCFS(std::vector<Process> processes) {
    validateProcesses(processes);
    sortByArrivalThenPid(processes);

    Result result;
    result.name = "FIRST-COME, FIRST-SERVED (FCFS)";

    Time time = 0;

    for (auto& process : processes) {
        if (time < process.arrival) {
            time = process.arrival;
        }

        result.trace.push_back(event(time, process.pid, "started"));
        time += process.burst;

        process.remaining = 0;
        process.completion = time;
        result.trace.push_back(event(time, process.pid, "finished"));
    }

    result.processes = std::move(processes);
    finalizeMetrics(result.processes,
                    result.averageTurnaround,
                    result.averageWaiting);
    return result;
}

Result simulateSJF(std::vector<Process> processes) {
    validateProcesses(processes);
    sortByArrivalThenPid(processes);

    auto shorterJobFirst = [](const Process* lhs, const Process* rhs) {
        if (lhs->burst != rhs->burst) {
            return lhs->burst > rhs->burst;
        }
        if (lhs->arrival != rhs->arrival) {
            return lhs->arrival > rhs->arrival;
        }
        return lhs->pid > rhs->pid;
    };

    std::priority_queue<
        Process*,
        std::vector<Process*>,
        decltype(shorterJobFirst)> ready(shorterJobFirst);

    Result result;
    result.name = "SHORTEST JOB FIRST (SJF, NON-PREEMPTIVE)";

    const int processCount = static_cast<int>(processes.size());
    int completed = 0;
    int nextArrival = 0;
    Time time = 0;

    while (completed < processCount) {
        while (nextArrival < processCount &&
               processes[nextArrival].arrival <= time) {
            ready.push(&processes[nextArrival]);
            ++nextArrival;
        }

        if (ready.empty()) {
            time = processes[nextArrival].arrival;
            continue;
        }

        Process* current = ready.top();
        ready.pop();

        result.trace.push_back(event(time, current->pid, "started"));
        time += current->burst;

        current->remaining = 0;
        current->completion = time;
        result.trace.push_back(event(time, current->pid, "finished"));
        ++completed;
    }

    result.processes = std::move(processes);
    finalizeMetrics(result.processes,
                    result.averageTurnaround,
                    result.averageWaiting);
    return result;
}

Result simulateRoundRobin(std::vector<Process> processes, Time quantum) {
    validateProcesses(processes);

    if (quantum <= 0) {
        throw std::runtime_error("Time quantum must be positive.");
    }

    sortByArrivalThenPid(processes);

    Result result;
    result.name = "ROUND ROBIN (Quantum = " + std::to_string(quantum) + ")";

    std::queue<int> ready;
    const int processCount = static_cast<int>(processes.size());
    int completed = 0;
    int nextArrival = 0;
    Time time = 0;

    auto enqueueArrivalsThrough = [&](Time currentTime) {
        while (nextArrival < processCount &&
               processes[nextArrival].arrival <= currentTime) {
            ready.push(nextArrival);
            ++nextArrival;
        }
    };

    while (completed < processCount) {
        enqueueArrivalsThrough(time);

        if (ready.empty()) {
            time = processes[nextArrival].arrival;
            enqueueArrivalsThrough(time);
        }

        const int processIndex = ready.front();
        ready.pop();

        Process& current = processes[processIndex];
        result.trace.push_back(event(time, current.pid, "started"));

        const Time slice = std::min(quantum, current.remaining);
        const Time endTime = time + slice;
        current.remaining -= slice;

        // Arrivals strictly before the end of the slice enter the queue now.
        while (nextArrival < processCount &&
               processes[nextArrival].arrival < endTime) {
            ready.push(nextArrival);
            ++nextArrival;
        }

        time = endTime;

        if (current.remaining == 0) {
            current.completion = time;
            result.trace.push_back(event(time, current.pid, "finished"));
            ++completed;

            // Any process arriving exactly at this completion time is now ready.
            enqueueArrivalsThrough(time);
        } else {
            // Coursework rule: if a process arrives exactly when the quantum
            // expires, enqueue that arrival BEFORE the preempted process.
            enqueueArrivalsThrough(time);

            result.trace.push_back(event(time, current.pid, "preempted"));
            ready.push(processIndex);
        }
    }

    result.processes = std::move(processes);
    finalizeMetrics(result.processes,
                    result.averageTurnaround,
                    result.averageWaiting);
    return result;
}

void printResult(const Result& result, std::ostream& out) {
    out << "--- " << result.name << " ---\n\n";

    out << "Execution Trace:\n";
    for (const auto& entry : result.trace) {
        out << entry << '\n';
    }

    out << "\nMetrics:\n";
    out << std::left
        << std::setw(8) << "PID"
        << std::setw(8) << "AT"
        << std::setw(8) << "BT"
        << std::setw(8) << "CT"
        << std::setw(8) << "TT"
        << std::setw(8) << "WT" << '\n';

    for (const auto& process : result.processes) {
        out << std::left
            << std::setw(8) << process.pid
            << std::setw(8) << process.arrival
            << std::setw(8) << process.burst
            << std::setw(8) << process.completion
            << std::setw(8) << process.turnaround
            << std::setw(8) << process.waiting << '\n';
    }

    out << std::fixed << std::setprecision(2)
        << "\nAverage Turnaround Time: "
        << result.averageTurnaround << '\n'
        << "Average Waiting Time: "
        << result.averageWaiting << "\n\n";
}
