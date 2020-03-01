prodcon: prodcon.o tands.o
	gcc -Wall -pthread -o prodcon prodcon.o tands.o

prodcon.o: prodcon.c tands.h
	gcc -Wall -pthread -c prodcon.c

tands.o: tands.c 
	gcc -Wall -c tands.c

clean:
	rm -f prodcon *.o