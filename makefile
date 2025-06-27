EMCC = em++
SRC_DIR = src/cpp
OUT_DIR = build
TARGET = $(OUT_DIR)/main.js

CPPFLAGS = -I$(SRC_DIR)
CXXFLAGS = -O0 -std=c++20 -gsource-map
EMFLAGS = -s WASM=1 -s MODULARIZE=1 -s EXPORT_NAME="SolverModule" \
          --bind --no-entry -s ALLOW_MEMORY_GROWTH=1 -fexceptions \
          -gsource-map --source-map-base http://localhost:5500/src/cpp/

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SOURCES)
	mkdir -p $(OUT_DIR)
	$(EMCC) $(CPPFLAGS) $(CXXFLAGS) $(EMFLAGS) $^ -o $@

clean:
	rm -rf $(OUT_DIR)/*.js $(OUT_DIR)/*.wasm $(OUT_DIR)/*.map
# EMCC = em++
# SRC_DIR = src/cpp
# OUT_DIR = build
# TARGET = $(OUT_DIR)/main.js

# CPPFLAGS = -I$(SRC_DIR)
# CXXFLAGS = -O0 -std=c++20 -gsource-map -g3 -fdebug-compilation-dir=.
# EMFLAGS = -s WASM=1 -s MODULARIZE=1 -s EXPORT_NAME="SolverModule" \
#           --bind --no-entry -s ALLOW_MEMORY_GROWTH=1 -fexceptions \
#           -gsource-map --source-map-base http://localhost:5500/ \
#           -g3 -s ASSERTIONS=1 -s SAFE_HEAP=1 \
#           --emit-symbol-map

# SOURCES = $(wildcard $(SRC_DIR)/*.cpp)

# .PHONY: all clean

# all: $(TARGET)

# $(TARGET): $(SOURCES)
# 	mkdir -p $(OUT_DIR)
# 	$(EMCC) $(CPPFLAGS) $(CXXFLAGS) $(EMFLAGS) $^ -o $@

# clean:
# 	rm -rf $(OUT_DIR)/*.js $(OUT_DIR)/*.wasm $(OUT_DIR)/*.map $(OUT_DIR)/*.symbols