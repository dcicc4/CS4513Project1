# Makefile for CS 4513 Project 1

FLAGS = -g -Wall

default: clean
default: rm dv dump

exp: clean
exp: FLAGS = -g -Wall -DAS_LIB
exp: rm.o dv.o dump.o experiments

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

# EXPERIMENTS
export.a : rm.o dv.o dump.o util.o futil.o
	ar -cvq export.a rm.o dv.o dump.o util.o futil.o

experiments: experiments.o export.a
	$(CC) experiments.o export.a -o experiments

experiments.o: experiments.c
	$(CC) $(FLAGS) -c experiments.c -o experiments.o

# DOCS AND CLEANUP
docs:
	doxygen

clean:
	/bin/rm -f *.o *.a rm dv dump experiments
