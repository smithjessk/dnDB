CXX_DEFAULT = g++
CXX_FANCY = clang++

COMPILER_OPTIONS = -Wall -std=c++11
LINKED_LIBRARIES = -lpthread -lboost_system -lboost_thread -lzmq -lcurl
NORMAL_FLAGS = $(COMPILER_OPTIONS) $(LINKED_LIBRARIES)
DEBUG_FLAGS = -O0 -g

MAIN_EXEC = bin/server
CLI_EXEC = bin/cli

all:
	mkdir -p bin
	$(CXX_DEFAULT) -o $(MAIN_EXEC) src/server.cc $(NORMAL_FLAGS)
	$(CXX_DEFAULT) -o $(CLI_EXEC) deps/jsoncpp.cpp src/interface.cc $(NORMAL_FLAGS)

clean:
	rm -rf bin

fancy:
	mkdir -p bin
	$(CXX_FANCY) -o $(MAIN_EXEC) src/server.cc $(NORMAL_FLAGS)
	$(CXX_FANCY) -o $(CLI_EXEC) deps/jsoncpp.cpp src/interface.cc $(NORMAL_FLAGS)

debug:
	mkdir -p bin
	$(CXX_FANCY) -o $(MAIN_EXEC) src/server.cc $(DEBUG_FLAGS) $(NORMAL_FLAGS)

tests:
	mkdir -p bin
	$(CXX_FANCY) -o bin/server_test deps/jsoncpp.cpp test/server_test.cc $(NORMAL_FLAGS)