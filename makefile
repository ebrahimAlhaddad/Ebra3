CC=g++

all: serial_o
serial_o: p1_serial.cpp
	g++ -lrt -fopenmp -o p1_a p1_serial.cpp

.PHONY: clean

clean:
