CXXFLAGS=-ggdb3 -Ofast -flto

all: poll-pps

poll-pps: poll-pps.o
	g++ $(CXXFLAGS) -o poll-pps poll-pps.o -lwiringPi -lc
