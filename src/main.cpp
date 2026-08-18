#include "scheduler.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

Time parseQuantum(const std::string& value) {
    std::size_t parsedCharacters = 0;
    Time quantum = 0;

    try {
        quantum = std::stoll(value, &parsedCharacters);
    } catch (const std::exception&) {
        throw std::runtime_error(
            "Time quantum must be a positive integer.");
    }

    if (parsedCharacters != value.size() || quantum <= 0) {
        throw std::runtime_error(
            "Time quantum must be a positive integer.");
    }

    return quantum;
}

void printUsage(const char* programName) {
    std::cout
        << "CPU Scheduling Simulator\n\n"
        << "Usage:\n"
        << "  " << programName << " <input_file> <time_quantum>\n\n"
        << "Input format:\n"
        << "  PID ArrivalTime BurstTime\n\n"
        << "Example:\n"
        << "  " << programName << " examples/tasks.txt 4\n";
}

}  // namespace

int main(int argc, char* argv[]) {
    if (argc == 2 && std::string(argv[1]) == "--help") {
        printUsage(argv[0]);
        return 0;
    }

    if (argc != 3) {
        printUsage(argv[0]);
        return 1;
    }

    try {
        const std::string inputFile = argv[1];
        const Time quantum = parseQuantum(argv[2]);

        const auto processes = readProcesses(inputFile);

        printResult(simulateFCFS(processes), std::cout);
        printResult(simulateSJF(processes), std::cout);
        printResult(simulateRoundRobin(processes, quantum), std::cout);
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
