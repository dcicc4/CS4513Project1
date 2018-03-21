# Makefile for CS 4513 Project 1

FLAGS = -g -Wall

all: rm dv dump experiments

# UTILTIES
util.o : util.c
	$(CC) -c util.c -o util.o

futil.o : futil.c
	$(CC) -c futil.c -o futil.o

# RM
rm : rm.o util.o futil.o
	$(CC) rm.o util.o futil.o -o rm

rm.o : rm.c
	$(CC) $(FLAGS) -c rm.c -o rm.o

# DV
dv : dv.o util.o futil.o
	$(CC) dv.o util.o futil.o -o dv

dv.o : dv.c
	$(CC) $(FLAGS) -c dv.c -o dv.o

# DUMP
dump : dump.o util.o
	$(CC) dump.o util.o -o dump

dump.o : dump.c
	$(CC) $(FLAGS) -c dump.c -o dump.o

# experiments
experiments: experiments.o util.o
	$(CC) experiments.o util.o -o experiments

experiments.o: experiments.c
	$(CC) $(FLAGS) -c experiments.c -o experiments.o

# DOCS AND CLEANUP
docs:
	doxygen

clean:
	/bin/rm *.o rm dv dump experiments
