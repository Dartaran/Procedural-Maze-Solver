# This is the Lambrusco Makefile 
# Author: Kyu Kim & Alan Lu & Donald Wilson
# Date: 3/2/16
# CS 50
# Assignment: Amazing Project

CC = gcc
CFLAGS = -Wall -pedantic -std=c11
DEBUG = -g -ggdb

all: amazing AMStartup

amazing: ./src/amazing.c 
	$(CC) $(CFLAGS) -o ./bin/$@ ./src/amazing.c

AMStartup: ./src/AMStartup.c 
	$(CC) $(CFLAGS) -o ./bin/$@ ./src/AMStartup.c

debug:
	$(CC) $(CFLAGS) $(DEBUG) -o ./bin/amazing ./src/amazing.c
	$(CC) $(CFLAGS) $(DEBUG) -o ./bin/AMStartup ./src/AMStartup.c

clean:
	rm -f ./bin/*~
	rm -f ./bin/*#
	rm -f ./bin/*.o
	./results/Cleanup.sh
