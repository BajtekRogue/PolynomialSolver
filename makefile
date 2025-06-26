
EMCC = em++
SRC_DIR = src/cpp
OUT_DIR = build
TARGET = $(OUT_DIR)/main.js

CPPFLAGS = -I$(SRC_DIR)
CXXFLAGS = -O2 -std=c++20
EMFLAGS = -s WASM=1 -s MODULARIZE=1 -s EXPORT_NAME="SolverModule" \
          --bind --no-entry -s ALLOW_MEMORY_GROWTH=1 -fexceptions

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SOURCES)
	mkdir -p $(OUT_DIR)
	$(EMCC) $(CPPFLAGS) $(CXXFLAGS) $(EMFLAGS) $^ -o $@

clean:
	rm -rf $(OUT_DIR)/*.js $(OUT_DIR)/*.wasm $(OUT_DIR)/*.map
