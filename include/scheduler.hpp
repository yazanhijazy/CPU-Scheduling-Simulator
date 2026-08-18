#pragma once

#include <iosfwd>
#include <string>
#include <vector>

using Time = long long;

struct Process {
    int pid{};
    Time arrival{};
    Time burst{};
    Time remaining{};
    Time completion{};
    Time turnaround{};
    Time waiting{};
};

struct Result {
    std::string name;
    std::vector<std::string> trace;
    std::vector<Process> processes;
    double averageTurnaround{};
    double averageWaiting{};
};

std::vector<Process> readProcesses(const std::string& filename);

Result simulateFCFS(std::vector<Process> processes);
Result simulateSJF(std::vector<Process> processes);
Result simulateRoundRobin(std::vector<Process> processes, Time quantum);

void printResult(const Result& result, std::ostream& out);
