CFLAGS = -std=c++11 -lpthread -lboost_system

all:
	mkdir -p bin
	$(CXX) -o bin/server src/server.cc $(CFLAGS)