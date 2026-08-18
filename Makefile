CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -O2
CPPFLAGS ?= -Iinclude

TARGET := scheduler
TEST_TARGET := scheduler_tests

SOURCES := src/main.cpp src/scheduler.cpp
TEST_SOURCES := tests/test_scheduler.cpp src/scheduler.cpp

.PHONY: all test clean

all: $(TARGET)

$(TARGET): $(SOURCES) include/scheduler.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

$(TEST_TARGET): $(TEST_SOURCES) include/scheduler.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(TEST_SOURCES) -o $(TEST_TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -f $(TARGET) $(TEST_TARGET)
