/* ========================================================================== */
/* File: AMStartup.c - The startup program for the Amazing Project
 *
 * Author: Alan Lu
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

// ---------------- Local includes  e.g., "file.h"
#include "amazing.h"
#include "utils.h"
#include "AMStartup.h"

// ---------------- Constant definitions
#define HOST_NAME "flume.cs.dartmouth.edu"

// ---------------- Macro definitions

// ---------------- Structures/Types

// ---------------- Private variables

/* ========================================================================== */

int main(int argc, char **argv) {
	/* check arguments */
	if (argc != 3) {
		printf("Usage: ./AMStartup [nAvatars] [Difficulty] [Hostname]\n");
		return 1;
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

	if (strcmp(HOST_NAME, argv[2]) != 0) {
		printf("Error: the hostname must be %s\n", HOST_NAME);
		return 1;
	}

	/* send AM_INIT message to server */
	AM_Message *init = malloc(sizeof(AM_Message));
	MALLOC_CHECK(stderr, init);
	init->init.nAvatars = htonl(nAvatars);
	init->init.Difficulty = htonl(difficulty);

	int sockfd; // socket file descriptor

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Error: failed to create the socket\n");
		return 1;
	}

	// get IP from hostname
	char *ip = malloc(sizeof(char));
	if (getIPFromHostName(hostname, ip) == 1) {
		return 1;
	}

	// create the socket address
	struct sockaddr_in socketAddress;
	memset(&socketAddress, 0, sizeof(socketAddress));
	socketAddress.sin_family = AF_INET;
	socketAddress.sin_port = htons(atoi(AM_SERVER_PORT));
	socketAddress.sin_addr.s_addr = inet_addr(ip);

	// attempt to connect to the server
	if (connect(sockfd, (struct sockaddr*) &socketAddress, sizeof(socketAddress)) < 0) {
		printf("Error: could not connect to the server: %s.\n", strerror(errno));
		return 1;
	}	

	// send the message
	send(sockfd, init, sizeof(AM_Message), 0);
	
	// receive the message
	AM_Message *initResponse = malloc(sizeof(AM_Message));
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

	free(initResponse);

	if (ntohs(initResponse->type) != AM_INIT_OK) {
		if (ntohs(initResponse->type) == AM_INIT_FAILED) {
			printf("Server initialization failed: %"PRIu32"\n", initResponse->init_failed.ErrNum);
			return 1;	
		}
		else {
			printf("Unknown serve rerror.\n");
			return 1;
		}
	}

	// get the necessary data from the init response
	unsigned int mazePort = ntohl(initResponse->init_ok.MazePort);
	unsigned int mazeWidth = ntohl(initResponse->init_ok.MazeWidth);
	unsigned int mazeHeight = ntohl(initResponse->init-ok.MazeHeight);

	// create log file
	FILE *logFile;
	char *fileName = malloc(sizeof(char));
	MALLOC_CHECK(stderr, fileName);
	char *userName = getenv("USER");
	MALLOC_CHECK(stderr, userName);

	sprintf(fileName, "Amazing_%s_%s_%s.log", userName, argv[1], argv[2]);
	logFile = fopen(filename, "w");
	MALLOC_CHECK(logFile);

	time_t now;
	time(&now);

	// write log file
	fprintf(logFile, "%s %i %s", userName, mazePort, ctime(&now));

	// startup avatar processes
	for (int i = 0; i < nAvatar; i++) {
		char *startAvatarCmd = malloc(sizeof(char));
		sprintf(startAvatarCmd, "./amazing_client %i %i %i %s %i %s", i, nAvatars, difficulty, ip, mazePort, fileName);
		system(startAvatarCmd); 
	}

	return 0;
}

int getIPFromHostName(char *hostname, char *ip) {
	struct hostent *he = gethostbyname(hostname);

	if (he == NULL) {
		printf("Error: could not get ip from hostname.\n");
		return 1;
	}

	struct in_addr addr;
	memcpy(&addr, he->h_addr_list[0], sizeof(struct in_addr));
	strncpy(ip, inet_ntoa(addr), AM_MAX_MESSAGE);

	return 0;
}
