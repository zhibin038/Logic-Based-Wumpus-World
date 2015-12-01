# Makefile

CC = c++ -g -Wall
OBJECTS = Action.o Agent.o Location.o Orientation.o Percept.o WumpusWorld.o Search.o Prover.o

all: wumpsim

wumpsim: wumpsim.h wumpsim.cc $(OBJECTS)
	$(CC) -o wumpsim wumpsim.cc $(OBJECTS)

Action.o: Action.h Action.cc
	$(CC) -c Action.cc

Agent.o: Agent.h Agent.cc
	$(CC) -c Agent.cc

Location.o: Location.h Location.cc
	$(CC) -c Location.cc

Orientation.o: Orientation.h Orientation.cc
	$(CC) -c Orientation.cc

Percept.o: Percept.h Percept.cc
	$(CC) -c Percept.cc

WumpusWorld.o: WumpusWorld.h WumpusWorld.cc
	$(CC) -c WumpusWorld.cc

Search.o: Search.h Search.cc
	$(CC) -c Search.cc

Prover.o: Prover.h Prover.cc
	$(CC) -c Prover.cc

clean:
	rm -f *.o wumpsim
