CFLAGS = -g -Wall -std=c++11
LDFLAGS = -lncurses


apple1: main.cpp CPU.h CPU.cpp RAM.h RAM.cpp IO.h IO.cpp Logger.h Logger.cpp
	g++ -o apple1 $(CFLAGS) main.cpp CPU.cpp RAM.cpp IO.cpp Logger.cpp $(LDFLAGS)
