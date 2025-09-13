EMCC = em++
SRC_DIR = src
OUT_DIR = docs
TARGET = $(OUT_DIR)/main.js

CPPFLAGS = -I$(SRC_DIR) 
CXXFLAGS = -O0 -std=c++20 -gsource-map 

CXXFLAGS += -DENABLE_ALL_LOGGING
CXXFLAGS += -DUSE_BIG_RATIONAL_TYPE

EMFLAGS = -s WASM=1 -s MODULARIZE=1 -s EXPORT_NAME="SolverModule" \
          --bind --no-entry -s ALLOW_MEMORY_GROWTH=1 -fexceptions \
          -gsource-map --source-map-base http://localhost:5500/src/cpp/ \
		  -s USE_BOOST_HEADERS=1 \
          -s INITIAL_MEMORY=64MB -s MAXIMUM_MEMORY=2GB

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SOURCES)
	mkdir -p $(OUT_DIR)
	$(EMCC) $(CPPFLAGS) $(CXXFLAGS) $(EMFLAGS) $^ -o $@

clean:
	rm -rf $(OUT_DIR)/*.js $(OUT_DIR)/*.wasm $(OUT_DIR)/*.map
