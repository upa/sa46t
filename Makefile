# Makefile for sa46t

CC = gcc -Wall -g
PROGNAME = sa46t
MODULES = iftun.o inetcon.o

.PHONY: all
all: $(PROGNAME)

.c.o:
	$(CC) -c $< -o $@

sa46t: sa46t.c $(MODULES)
	$(CC) sa46t.c $(MODULES) -o $@


clean:
	rm $(MODULES) $(PROGNAME)


iftun.o: iftun.h iftun.c
ientcon.o: inetcon.h inetcon.c
