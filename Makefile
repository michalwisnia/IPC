CC=gcc
CCFLAGS=-Wall

all: serwer klient

serwer: inf141335_s.c
	${CC} ${CCFLAGS} $< -o $@

klient: inf141335_k.c
	${CC} ${CCFLAGS} $< -o $@

clean:
	rm serwer
	rm klient
