all: macspoof.o utils.o network.o
	gcc -g -Wall macspoof.o utils.o network.o -lm -o run

macspoof.o: macspoof.c
	gcc -g -Wall -c macspoof.c

utils.o: utils.h utils.c
	gcc -g -Wall -c utils.c

network.o: network.h network.c
	gcc -g -Wall -c network.c

clean:
	rm -f macspoof.o utils.o network.o all