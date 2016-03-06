# This is the Lambrusco Makefile 
# Author: Kyu Kim & Alan Lu & Donald Wilson
# Date: 3/2/16
# CS 50
# Assignment: Amazing Project

CC = gcc
CFLAGS = -Wall -pedantic -std=c11
DEBUG = -g -ggdb

all: amazing_client AMStartup

amazing_client: ./src/amazing_client.c 
	$(CC) $(CFLAGS) -o ./bin/$@ ./src/amazing_client.c ./src/utils.c

AMStartup: ./src/AMStartup.c 
	$(CC) $(CFLAGS) -o ./bin/$@ ./src/AMStartup.c ./src/utils.c

debug:
	$(CC) $(CFLAGS) $(DEBUG) -o ./bin/amazing_client ./src/amazing_client.c ./src/utils.c
	$(CC) $(CFLAGS) $(DEBUG) -o ./bin/AMStartup ./src/AMStartup.c ./src/utils.c

clean:
	rm -f ./bin/*
	cd results; ./Cleanup.sh
