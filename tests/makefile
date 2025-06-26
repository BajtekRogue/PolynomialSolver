CXX = g++
SRC_DIR = ../src/cpp
BUILD_DIR = build

CXXFLAGS = -std=c++17 -Wall -Wextra -Wno-sign-compare -I$(SRC_DIR) -pthread
LIBS = -lgtest_main -lgtest -pthread

TEST_SOURCES := $(wildcard test_*.cpp)

SRC_SOURCES := $(filter-out $(SRC_DIR)/Emscripten.cpp, $(wildcard $(SRC_DIR)/*.cpp))

TEST_BINARIES := $(patsubst %.cpp, $(BUILD_DIR)/%, $(TEST_SOURCES))

all: $(BUILD_DIR) $(TEST_BINARIES)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%: %.cpp $(SRC_SOURCES)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)

run: all
	@for testbin in $(TEST_BINARIES); do \
		echo "Running $$testbin..."; \
		./$$testbin || exit 1; \
	done

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all run clean
