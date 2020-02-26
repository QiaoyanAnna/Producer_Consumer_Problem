prodcon: prodcon.o tands.o

prodcon.o: prodcon.c tands.h
	gcc -Wall -std=c99 -c prodcon.c

tands.o: tands.c 
	gcc -Wall -std=c99 -c tands.c

clean:
	rm -f prodcon *.o