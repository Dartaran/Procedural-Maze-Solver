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

// ---------------- Local includes  e.g., "file.h"

// ---------------- Constant definitions
#define HOST_NAME = "flume.cs.dartmouth.edu"

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
	MALLOC_CHECK(init);
	init->init.nAvatars = htonl(nAvatars);
	init->init.Difficulty = htonl(difficulty);

	int sockfd; // socket file descriptor

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Error: failed to create the socket\n");
		return 1;
	}

	// get IP from hostname
	char *ip;
	if (getIPFromHostName(hostname, ip) == 1) {
		return 1;
	}


	// create the socket address
	struct sockaddr *socketAddress = malloc(sizeof(sockaddr));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(AM_SERVER_PORT);
	serverAddress.sin_addr.s_addr = inet_addr(ip);

	// attempt to connect to the server
	if (connect(sockfd, socketAddress, sizeof(socketAddress)) < 0) {
		printf("Error: could not connect to the server: %s.\n", strerror(errno));
		return 1;
	}	

	// send the message
	send(sockfd, init, sizeof(AM_MESSAGE), 0);

		
}

int getIPFromHostName(char *hostname, char *ip) {
	struct hostent *he = gethostbyname(hostname);

	if (he == NULL) {
		printf("Error: could not get ip from hostname.\n");
		return 1;
	}

	struct in_addr addr = malloc(sizeof(in_addr));
	strncpy(ip, inet_ntoa(addr), AM_MAX_MESSAGE);

	return 0;
}
