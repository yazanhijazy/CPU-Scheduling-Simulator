# Project Notes

## Original Version

The uploaded project contained:

- `main.cpp`
- `Makefile`
- `tasks.txt`
- `README.md`

The original implementation already supported FCFS, non-preemptive SJF, and Round Robin and compiled successfully with C++17.

## GitHub-Ready Improvements

The portfolio version keeps the original scheduling behavior while improving project organization and validation:

- split the simulator into a reusable scheduling module and CLI entry point
- moved sample input to `examples/`
- added CMake support while retaining Make
- added automated tests
- added a GitHub Actions build/test workflow
- added `.gitignore` and `.gitattributes`
- upgraded time-related values to `long long`
- rejects duplicate/non-positive PIDs
- rejects malformed lines with extra tokens
- ignores blank lines and comment lines
- validates the complete time-quantum string instead of accepting partially numeric values
- documents the Round Robin exact-boundary arrival rule explicitly

No external libraries are required.
