default: dump rm dv

dump.o: dump.c
	gcc -c dump.c -o dump.o

dump: dump.o
	gcc dump.o -o dump

rm.o: rm.c
	gcc -c rm.c -o rmp.o

rm: rm.o
	gcc rm.o -o rm

dv.o: dv.c
	gcc -c dv.c -o dv.o

dv: dv.o
	gcc dv.o -o rm

clean: 
	-rm -f *.o
	-rm -f dump 
	-rm -f rm 
	-rm -f dv
