#include "scheduler.hpp"

#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {

const Process& byPid(const Result& result, int pid) {
    for (const auto& process : result.processes) {
        if (process.pid == pid) {
            return process;
        }
    }
    throw std::runtime_error("PID not found in result.");
}

void testCourseworkExample() {
    const std::vector<Process> processes = {
        {1, 0, 24, 24},
        {2, 1, 3, 3},
        {3, 2, 3, 3},
    };

    const auto fcfs = simulateFCFS(processes);
    assert(byPid(fcfs, 1).completion == 24);
    assert(byPid(fcfs, 2).completion == 27);
    assert(byPid(fcfs, 3).completion == 30);
    assert(std::abs(fcfs.averageWaiting - 16.0) < 1e-9);

    const auto sjf = simulateSJF(processes);
    assert(byPid(sjf, 1).completion == 24);
    assert(byPid(sjf, 2).completion == 27);
    assert(byPid(sjf, 3).completion == 30);

    const auto rr = simulateRoundRobin(processes, 4);
    assert(byPid(rr, 1).completion == 30);
    assert(byPid(rr, 2).completion == 7);
    assert(byPid(rr, 3).completion == 10);
}

void testSjfChoosesShortestReadyJob() {
    const std::vector<Process> processes = {
        {1, 0, 5, 5},
        {2, 0, 2, 2},
        {3, 0, 3, 3},
    };

    const auto result = simulateSJF(processes);

    assert(byPid(result, 2).completion == 2);
    assert(byPid(result, 3).completion == 5);
    assert(byPid(result, 1).completion == 10);
}

void testIdleCpuGap() {
    const std::vector<Process> processes = {
        {1, 5, 2, 2},
    };

    const auto fcfs = simulateFCFS(processes);
    const auto sjf = simulateSJF(processes);
    const auto rr = simulateRoundRobin(processes, 3);

    assert(byPid(fcfs, 1).completion == 7);
    assert(byPid(sjf, 1).completion == 7);
    assert(byPid(rr, 1).completion == 7);

    assert(byPid(fcfs, 1).waiting == 0);
    assert(byPid(sjf, 1).waiting == 0);
    assert(byPid(rr, 1).waiting == 0);
}

void testRoundRobinBoundaryArrivalRule() {
    const std::vector<Process> processes = {
        {1, 0, 8, 8},
        {2, 4, 1, 1},
    };

    // At time 4, P2 arrives exactly when P1's quantum expires.
    // The required rule places P2 in the queue before re-queueing P1.
    const auto result = simulateRoundRobin(processes, 4);

    assert(byPid(result, 2).completion == 5);
    assert(byPid(result, 1).completion == 9);
}

void testInvalidQuantum() {
    const std::vector<Process> processes = {
        {1, 0, 1, 1},
    };

    bool threw = false;

    try {
        (void)simulateRoundRobin(processes, 0);
    } catch (const std::runtime_error&) {
        threw = true;
    }

    assert(threw);
}

}  // namespace

int main() {
    testCourseworkExample();
    testSjfChoosesShortestReadyJob();
    testIdleCpuGap();
    testRoundRobinBoundaryArrivalRule();
    testInvalidQuantum();

    std::cout << "All scheduler tests passed.\n";
    return 0;
}
