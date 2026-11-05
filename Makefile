CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -pthread

all: build/server build/client

build:
	mkdir -p build

build/server: src/server.cpp src/InventoryManager.cpp src/Item.cpp src/InventoryManager.h src/Item.h | build
	$(CXX) $(CXXFLAGS) src/server.cpp src/InventoryManager.cpp src/Item.cpp -o $@

build/client: src/client.cpp | build
	$(CXX) $(CXXFLAGS) src/client.cpp -o $@

clean:
	rm -rf build

.PHONY: all clean
