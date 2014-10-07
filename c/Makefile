CC = gcc
#CFLAGS = -g -Wall -Wextra
CFLAGS = -g -Wall

all: automata

automata: queue.o list.o

clean:
	rm -f *.o automata

test: automata
	cat ../test/conf.txt | ./automata | dot -Tpng -o ../test/dot.png
	firefox ../test/dot.png &
