CC=g++

all: p1_serial p1_a p1_b p2_serial p2
p1_a: p1_a.cpp
	g++ -lrt -fopenmp -o p1_a p1_a.cpp

p1_b: p1_b.cpp
	g++ -lrt -fopenmp -o p1_b p1_b.cpp

p1_serial: p1_serial.c
	g++ -o p1_serial p1_serial.c

p2_serial: p2_serial.cpp
	g++ -o p2_serial p2_serial.cpp

p2: p2.cpp
	g++ -lrt -fopenmp -o p2 p2.cpp

.PHONY: clean

clean:
