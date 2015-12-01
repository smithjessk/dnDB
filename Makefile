CXX_DEFAULT = g++
CXX_FANCY = clang++
NORMAL_FLAGS = -Wall -std=c++11 -lpthread -lboost_system -lboost_thread
DEBUG_FLAGS = -O0 -g

all:
	mkdir -p bin
	$(CXX_DEFAULT) -o bin/server src/server.cc $(NORMAL_FLAGS)

clean:
	rm -rf bin

fancy:
	mkdir -p bin
	$(CXX_FANCY) -o bin/server src/server.cc $(NORMAL_FLAGS)

debug:
	mkdir -p bin
	$(CXX_FANCY) -o bin/server src/server.cc $(DEBUG_FLAGS) $(NORMAL_FLAGS)