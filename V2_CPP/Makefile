
CC = g++
CFLAGS = -std=c++17

all: lib-setup

lib-setup: lib-setup.cpp setup-funcs.o
	$(CC) $(CFLAGS) -o lib-setup lib-setup.cpp setup-funcs.o

setup-funcs.o: setup-funcs.cpp setup-funcs.h
	$(CC) $(CFLAGS) -c setup-funcs.cpp

clean:
	rm -f lib-setup setup-funcs.o
