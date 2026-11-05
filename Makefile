# A.I. Disclaimer: written with help from Claude (Anthropic), an AI assistant.
#
#   make          build ./atc and ./test_queues
#   make test     run the unit tests
#   make check    check every finished structure lands flights in the real order
#   make bench    time every finished structure (writes results/results.csv)

CXX      = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -Isrc

OBJS    = src/Flight.o src/Simulation.o src/SortedVectorQueue.o src/LazyHeapQueue.o src/IndexedHeapQueue.o
HEADERS = $(wildcard src/*.h)

all: atc test_queues

atc: src/main.o $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

test_queues: tests/test_queues.o $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Any header change rebuilds every object file: simple, and fast enough for a project this size.
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: test_queues
	./test_queues

check: atc
	./atc check

bench: atc
	./atc bench

clean:
	rm -f atc test_queues src/*.o tests/*.o

.PHONY: all test check bench clean
