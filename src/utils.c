/*	utils.c Helpful utility functions

	Project name: CS50 Amazing Project

	Primary Author:	Kyu Kim, Alan Lu, Donald Wilson (Lambrusco)
	Date Created:	2/28/16
	
======================================================================*/
// do not remove any of these sections, even if they are empty
//
// ---------------- Open Issues 

// ---------------- System includes e.g., <stdio.h>
#include <stdio.h>			// basic library functions
#include <stdlib.h>			// for atoi
#include <string.h>			// string functions
#include <errno.h>			// errno support
#include <sys/socket.h>		// socket networking functions
#include <arpa/inet.h>		// for htonl

// ---------------- Local includes  e.g., "file.h"
#include "utils.h"

// ---------------- Constant definitions 

// ---------------- Macro definitions

// ---------------- Structures/Types 

// ---------------- Private variables 

// ---------------- Private prototypes 

/*====================================================================*/

/*
 * Displays the contents of a text file.
 */
int DisplayFile(char *filepath)
{
	FILE *fp;
	char buf[10000];
  	if( (fp = fopen(filepath, "r+")) != NULL ) {
     	while(!feof (fp)) {
	  		fgets(buf, sizeof(buf), fp);
	  		fputs(buf, stdout);
		}
		printf("\n");
		fclose(fp);
    }
  	else {
      	printf ("Error opening file: %s\n", filepath);
      	return 1;
    }
  	return 0;
}

/*
* Connects to a server with specified socket, ip, and port.
*/
int ConnectToServer(char *serverIp, int port) {
	uint32_t sockFd;
	
	if ((sockFd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Error: failed to create the socket\n");
		return -1;	
	}

	// create the socket address
	struct sockaddr_in serverAddress;
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);
	serverAddress.sin_addr.s_addr = inet_addr(serverIp);

	if (connect(sockFd, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) < 0) {
		printf("Error: could not connect to the server: %s.\n", strerror(errno));
		return -1;
	}

	return sockFd;
}