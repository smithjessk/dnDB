CXX_DEFAULT = g++
CXX_FANCY = clang++
CFLAGS = -Wall -std=c++11 -lpthread -lboost_system

all:
	mkdir -p bin
	$(CXX_DEFAULT) -o bin/server src/server.cc $(CFLAGS)

clean:
	rm -rf bin

fancy:
	mkdir -p bin
	$(CXX_FANCY) -o bin/server src/server.cc $(CFLAGS)