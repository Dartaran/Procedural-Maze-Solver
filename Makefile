# This is the Lambrusco Makefile 
# Author: Kyu Kim & Alan Lu & Donald Wilson
# Date: 3/2/16
# CS 50
# Assignment: Amazing Project

CC = gcc
CFLAGS = -Wall -pedantic -std=c11

amazing: ./src/amazing.c 
	$(CC) $(CFLAGS) -o $@ ./src/amazing.c

AMStartup: ./src/AMStartup.c 
	$(CC) $(CFLAGS) -o $@ ./src/AMStartup.c

clean:
	rm -f *~
	rm -f *#
	rm -f *.o
