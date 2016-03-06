/* ========================================================================== */
/* File: AMStartup.c - The startup program for the Amazing Project
 *
 * Author: Alan Lu, Kyu Kim, Donald Wilson (Lambrusco)
 * Date: February 28, 2016
 *
 * Input: [nAvatars] [Difficulty] [Hostname]
 *
 * Command line options: N/A
 *
 * Output: Starts a number of avatar clients specified by nAvatars, passing the 
 * correct parameters as returned by the server.
 *
 * Error Conditions: N/A
 *
 * Special Considerations: N/A
 *
 */
/* ========================================================================== */
// ---------------- Open Issues

// ---------------- System includes e.g., <stdio.h>
#include <stdio.h>			// basic library functions
#include <stdlib.h>			// for atoi
#include <sys/socket.h>		// socket networking functions
#include <arpa/inet.h>		// for htonl
#include <string.h>			// string functions
#include <netdb.h>
#include <errno.h>
#include <time.h>
#include <netinet/in.h>
#include <inttypes.h>

// ---------------- Local includes  e.g., "file.h"
#include "amazing.h"
#include "utils.h"
#include "AMStartup.h"

// ---------------- Constant definitions
#define HOST_NAME "flume.cs.dartmouth.edu"
#define HELPFILE "../help/AMStartup_help.txt"

// ---------------- Macro definitions

// ---------------- Structures/Types

// ---------------- Private variables

// ---------------- Private prototypes

/* ========================================================================== */

int main(int argc, char **argv) {
	/* check arguments */
	if (argc != 4) {
		if (2 == argc) {
			if ( 0 == strcmp("--help", argv[1]) ){
				DisplayFile(HELPFILE);
				return 0;
			}
			else {
				fprintf(stderr, "Usage: incorrect arguments. Use --help option for usage info.\n");
				return 1;
			}
		}
		else {
			fprintf(stderr, "Usage: ./AMStartup [nAvatars] [Difficulty] [Hostname]\n");
			return 1;
		}
	}

	int nAvatars = atoi(argv[1]);
	int difficulty = atoi(argv[2]); 
	char *hostname = argv[3];

	if (nAvatars < 2) {
		printf("Error: the minimum number of avatars is 2.\n");
		return 1;
	}
	else if (nAvatars > AM_MAX_AVATAR) {
		printf("Error: the maximum number of avatars is %i.\n", AM_MAX_AVATAR);
		return 1;
	}

	if (difficulty < 0) {
		printf("Error: the difficulty must be positive.\n");
		return 1;
	}
	else if (difficulty > AM_MAX_DIFFICULTY) {
		printf("Error: the maximum difficulty is %i.\n", AM_MAX_DIFFICULTY);
		return 1; 
	}

	/* send AM_INIT message to server */
	AM_Message *init = calloc(1, AM_MAX_MESSAGE + 1);
	MALLOC_CHECK(stderr, init);
	init->type = htonl(AM_INIT);
	init->init.nAvatars = htonl(nAvatars);
	init->init.Difficulty = htonl(difficulty);

	uint32_t sockfd; // socket file descriptor

	// get IP from hostname
	char *ip = calloc(1, AM_MAX_MESSAGE + 1);
	if (getIPFromHostName(hostname, ip) == 1) {
		return 1;
	}

	// attempt to connect to the server
	if ((sockfd = ConnectToServer(ip, atoi(AM_SERVER_PORT))) == -1) {
		return 1;
	}	

	// send the message
	send(sockfd, init, sizeof(AM_Message), 0);

	free(init);
	
	// receive the message
	AM_Message *initResponse = calloc(1, AM_MAX_MESSAGE + 1);
	MALLOC_CHECK(stderr, initResponse);

	if (recv(sockfd, initResponse, sizeof(AM_Message), 0) == 0) {
		printf("Server error\n");
		free(initResponse);
		return 1;
	}

	if (ntohs(initResponse->type) & AM_ERROR_MASK) { 
		printf("Corrupt message\n");
		return 1;
	}

	if (IS_AM_ERROR(ntohl(initResponse->type))) {
		if (ntohl(initResponse->type) == AM_INIT_FAILED) {
			printf("Server initialization failed: %"PRIu32"\n", ntohl(initResponse->init_failed.ErrNum));
			return 1;	
		}
		else {
			printf("Unknown server error.\n");
			return 1;
		}
	}

	// get the necessary data from the init response
	unsigned int mazePort = ntohl(initResponse->init_ok.MazePort);
	unsigned int mazeWidth = ntohl(initResponse->init_ok.MazeWidth);
	unsigned int mazeHeight = ntohl(initResponse->init_ok.MazeHeight);

	free(initResponse);

	// create log file
	FILE *logFile;
	char *userName = getenv("USER");
	MALLOC_CHECK(stderr, userName);
	char *fileName = calloc(30 + strlen(userName) + strlen(argv[1]) + strlen(argv[2]), sizeof(char));
	MALLOC_CHECK(stderr, fileName);

	sprintf(fileName, "../results/Amazing_%s_%s_%s.log", userName, argv[1], argv[2]);
	logFile = fopen(fileName, "w");
	MALLOC_CHECK(stderr, logFile);

	time_t now;
	time(&now);

	// write log file
	fprintf(logFile, "%s %i %s", userName, mazePort, ctime(&now));

	// startup avatar processes
	for (int i = 0; i < nAvatars; i++) {
		char startAvatarCmd[AM_MAX_MESSAGE];
		sprintf(startAvatarCmd, "./amazing_client %i %i %i %s %i %s &", i, nAvatars, difficulty, ip, mazePort, fileName);
		fprintf(logFile, "Starting avatar %i\n", i);
		fprintf(logFile, "%s\n", startAvatarCmd);
		system(startAvatarCmd);
	}

	free(fileName);
	free(ip);
	fclose(logFile);
	return 0;
}

/*
* Gets the ip address of a hostname. If no ip is found then returns an error.
*/
int getIPFromHostName(char *hostname, char *ip) {
	struct hostent *he = gethostbyname(hostname);

	if (he == NULL) {
		printf("Error: could not get ip from hostname.\n");
		return 1;
	}

	struct in_addr addr;
	memcpy(&addr, he->h_addr_list[0], sizeof(struct in_addr));
	strcpy(ip, inet_ntoa(addr));

	return 0;
}

