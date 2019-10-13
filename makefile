CC=g++

all: p1_a p1_b p1_serial.o p1_serial.ex
p1_a: p1_a.cpp
	g++ -lrt -fopenmp -o p1_a p1_a.cpp

p1_b: p1_b.cpp
	g++ -lrt -fopenmp -o p1_b p1_b.cpp

p1_serial.o: p1_serial.c
	gcc -c p1_serial.c

p1_serial.ex: p1_serial.o
	gcc -o p1_serial p1_serial.o
	
.PHONY: clean

clean:
