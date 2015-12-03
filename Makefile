CXX_DEFAULT = g++
CXX_FANCY = clang++
COMPILER_OPTIONS = -Wall -std=c++11
LINKED_LIBRARIES = -lpthread -lboost_system -lboost_thread -lzmq -lcurl
NORMAL_FLAGS = $(COMPILER_OPTIONS) $(LINKED_LIBRARIES)
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

tests:
	mkdir -p bin
	$(CXX_FANCY) -o bin/test deps/jsoncpp.cpp test/test.cc $(NORMAL_FLAGS)