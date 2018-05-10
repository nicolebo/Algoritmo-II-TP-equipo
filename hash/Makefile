CC = gcc
CFLAGS = -g -std=c99 -Wall -Wconversion -Wno-sign-conversion -Werror
VFLAGS = --leak-check=full --track-origins=yes --show-reachable=yes

build: main.c hash.c hash.h testing.c testing.h pruebas_catedra.c
	$(CC) $(CFLAGS) main.c hash.c hash.h testing.c pruebas_catedra.c -o pruebas

run: build
	./pruebas

valgrind: build
	valgrind $(VFLAGS) ./pruebas