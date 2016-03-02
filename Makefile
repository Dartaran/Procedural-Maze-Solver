# This is the Lambrusco Makefile 
# Author: Kyu Kim & Alan Lu & Donald Wilson
# Date: 3/2/16
# CS 50
# Assignment: Amazing Project

CC = gcc
CFLAGS = -Wall -pedantic -std=c11
CFILES= ./src/AMStartup.c ./src/amazing.c

amazing: ./src/amazing.c 
	$(CC) $(CFLAGS) -o amazing_client ./src/amazing.c

AMStartup: ./src/AMStartup.c 
	$(CC) $(CFLAGS) -o $@ ./src/AMStartup.c

clean:
	rm -f *~
	rm -f *#
	rm -f *.o
