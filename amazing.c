/*	amazing.c Maze solving program

	Project name: CS50 Amazing Project
	Component name: amazing program

	Primary Author:	Kyu Kim, Alan Lu, Donald Wilson (Lambrusco)
	Date Created:	2/28/16

	Special considerations:  
		(e.g., special compilation options, platform limitations, etc.) 
	
======================================================================*/
// do not remove any of these sections, even if they are empty
//
// ---------------- Open Issues 

// ---------------- System includes e.g., <stdio.h>
#include <stdio.h>			// basic library functions
#include <stdlib.h>			// for atoi
#include <sys/socket.h>		// socket networking functions
#include <arpa/inet.h>		// for htonl
#include <string.h>			// string functions

// ---------------- Local includes  e.g., "file.h"
#include "amazing.h"

// ---------------- Constant definitions 

// ---------------- Macro definitions

// ---------------- Structures/Types 

// ---------------- Private variables 

// ---------------- Private prototypes 

/*====================================================================*/

int main(int argc, char* argv[]) {

	// Declare local variables
	uint32_t avatarId, nAvatars, difficulty, serverIp, mazePort;
	char* logFile;				// filename

	uint32_t sockFd;
	struct sockaddr* serverAddress;

	uint32_t i, dir, nextDirection;
	struct *Avatar avatar;
	struct avatar_ready avatarReady;
	
	// Check args (checking number of args is sufficient)
	if( 7 != argc) {
		//TODO print error to log
		exit 1;
	}

	// Initialize local variables
	avatarId = argv[1];
	nAvatars = argv[2];
	difficulty = argv[3];
	serverIp = argv[4];
	mazePort = argv[5];
	//TODO logFile = malloc(...	//TODO free memory

			
	i = 0;						// a count of unsuccessful moves, such that 
								// 0 indicates a left turn; 1 ahead; 2 right; 3 back

	dir = M_NORTH;				// direction of last successful move
	nextDirection = M_WEST;		// direction to attempt next

	avatar->fd = AvatarID;
	avatar->pos->x = -1;		// initialized to impossible value
	avatar->pos->y = -1;


	if ((sockFd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Error: failed to create the socket\n");
		return 1;	

	// create the socket address
	serverAddress = malloc(sizeof(sockaddr));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(AM_SERVER_PORT);
	serverAddress.sin_addr.s_addr = inet_addr(serverIp);

	// attempt to connect to the server
	if (connect(sockFd, serverAddress, sizeof(serverAddress)) < 0) {
		//TODO print error to log
		return 1;
	}

	// Send AM_AVATAR_READY message
	avatarReady->AvatarId = htons(avatarId);
	send(MazePort, avatar_ready, sizeof(AM_MESSAGE), 0);

	// While we haven’t completed the maze:
	for(;;) {
	// TODO Listen for a message from the server

	// If AM_AVATAR_TURN:
		// If it’s my turn to move (i.e. TurnID == AvatarID):
		if(//TODO){
			// If my position changed:
			if(//TODO) {
				// Update position
				avatar->pos->x = //TODO;
				avatar->pos->y = //TODO;

				// Update direction of last successful move:
				dir = nextDirection;
				i = 0;
			}

			// (Note: We designate Avatar 0 as the “exit”)
			// If position == exit location (Avatar 0’s position):
			if(//TODO) {
				// Stay put; set nextDirection = 8 (null move)
			}
			else {
				nextDirection = (direction + 3 + i) mod 4;
			}

			// Attempt to make a move in the nextDirection
			//TODO
			
			i++;
		}

	// Else if AM_MAZE_SOLVED 
		// Avatar 0 writes a success message to log file
		// Close files, free allocated memory, etc
		// Exit

	// Else if AM_AVATAR_TOO_MANY_MOVES or AM_SERVER_TIMEOUT or 
	// AM_SERVER_OUT_OF_MEM
		// Avatar 0 writes message to log file
		// Close files, free allocated memory, etc
		// Exit
			
	// Else if AM_NO_SUCH_AVATAR or AM_UKNOWN_MSG_TYPE or 
	// AM_UNEXPECTED_MSG_TYPE or AM_AVATAR_OUT_OF_TURN
	// Write error message to log file

	// Else if AM_SERVER_DISK_QUOTA
		// Write error message to log file
		// (Note: Please notify TA/instructor in this event)
	}
	// Exit
}