# Makefile for CS 4513 Project 1

FLAGS = -g -Wall

all: rm dv dump

# RM
rm : rm.o
	$(CC) rm.o -o rm
	
rm.o : rm.c
	$(CC) $(FLAGS) -c rm.c -o rm.o
	
# DV
dv : dv.o
	$(CC) dv.o -o dv

dv.o : dv.c
	$(CC) $(FLAGS) -c dv.c -o dv.o

# DUMP
dump : dump.o
	$(CC) dump.o -o dump

dump.o : dump.c
	$(CC) $(FLAGS) -c dump.c -o dump.o

docs:
	doxygen
	
clean:
	/bin/rm *.o rm dv dump
