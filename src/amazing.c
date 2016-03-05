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
#include <errno.h>			// errno support
#include <inttypes.h>		// PRIu32

// ---------------- Local includes  e.g., "file.h"
#include "amazing.h"
#include "utils.h"

// ---------------- Constant definitions 

// ---------------- Macro definitions

// ---------------- Structures/Types 

// ---------------- Private variables 

// ---------------- Private prototypes 

/*====================================================================*/
int main(int argc, char* argv[]) {

	// Declare local variables
	unsigned int avatarId, nAvatars, difficulty, mazePort;
	char *serverIp;
	char* logFilePath;
	FILE* logFile;

	uint32_t sockFd;

	int i, dir, nextDirection;
	struct Avatar* avatar;
	struct AM_Message* avatarReady;
	struct AM_Message* avatarMove;
	ssize_t recvMessageLen;				// in bytes
	AM_Message* recvMessage;

	const int compass[] = {M_WEST, M_NORTH, M_EAST, M_SOUTH};
	
	// Check args (checking number of args is sufficient)
	if( 7 != argc) {
		// print error to fprintf
		fprintf(stderr, "Incorrect argument count for avatar %i.  Usage ./amazing [id] [nAvatars] [difficulty] [ip] [mazePort] [fileName]\n", atoi(argv[2]));
		exit (1);
	}

	// Initialize local variables
	avatarId = atoi(argv[1]);
	nAvatars = atoi(argv[2]);
	difficulty = atoi(argv[3]);
	serverIp = argv[4];
	mazePort = atoi(argv[5]);
	logFilePath = calloc(strlen(argv[6]) + 1, sizeof(char));
	strcpy(logFilePath, argv[6]);
	logFile = fopen(logFilePath, "a");
	MALLOC_CHECK(stderr, logFile);

	dir = M_NORTH;				// direction of last successful move
	nextDirection = M_NORTH;	// direction to attempt next
	i = 0;						// a count of unsuccessful moves, such that 
								// 0 indicates a left turn; 1 ahead; 2 right; 3 back

	avatar = malloc( sizeof(Avatar) );
	avatar->fd = avatarId;
	avatar->pos.x = -1;		// initialized to impossible value
	avatar->pos.y = -1;

	if ((sockFd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Error: failed to create the socket\n");
		fclose(logFile);
		free(avatar);
		return 1;	
	}

	// create the socket address
	struct sockaddr_in serverAddress;
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(mazePort);
	serverAddress.sin_addr.s_addr = inet_addr(serverIp);

	// attempt to connect to the server
	if (connect(sockFd, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) < 0) {
		printf("Error: could not connect to the server: %s.\n", strerror(errno));
		fclose(logFile);
		free(avatar);
		return 1;
	}
		
	// Send AM_AVATAR_READY message
	avatarReady = calloc(1, AM_MAX_MESSAGE + 1);
	avatarReady->type = htonl(AM_AVATAR_READY);
	avatarReady->avatar_ready.AvatarId = htonl(avatarId);
	send(sockFd, avatarReady, sizeof(AM_Message), 0);
	printf("Avatar ID %i sent avatar ready.\n", avatarId);

	// free send memory
	free(avatarReady);

	recvMessage = calloc(1, AM_MAX_MESSAGE + 1);
	avatarMove = calloc(1, AM_MAX_MESSAGE + 1);

	// While we haven’t completed the maze, listen for a message from the server:
	while( (recvMessageLen = recv(sockFd, recvMessage, AM_MAX_MESSAGE, 0) > 0) 	) {
		// evaluate message type
		if (IS_AM_ERROR(ntohl(recvMessage->type))){
			printf("Error: %"PRIu32" \n", ntohl(recvMessage->type));

			if(0 == avatarId) {
				if (ntohl(recvMessage->type) == AM_SERVER_OUT_OF_MEM) {
					printf("server out of memory.\n");
				}
				else if (ntohl(recvMessage->type) == AM_SERVER_DISK_QUOTA) {
					printf("server disk quota exceeded.\n");
				}
				else if (ntohl(recvMessage->type) == AM_UNEXPECTED_MSG_TYPE) {
					printf("unexpected message type.\n");
				}
				else if (ntohl(recvMessage->type) == AM_TOO_MANY_MOVES) {
					printf("too many moves.\n");
				}
				else if (ntohl(recvMessage->type) == AM_UNKNOWN_MSG_TYPE) {
					printf("unknown message type %i.\n", ntohl(recvMessage->unknown_msg_type.BadType));
				}
			}

			if (ntohl(recvMessage->type) == AM_NO_SUCH_AVATAR) {
				printf("no avatar with ID %i\n", avatarId);
			}
			else if (ntohl(recvMessage->type) == AM_AVATAR_OUT_OF_TURN) {
				printf("avatar %i out of turn.\n", avatarId);
			}

			// free allocated memory, etc
			free(recvMessage);
			free(avatar);
			fclose(logFile);
			return 1;
		}

		// If AM_AVATAR_TURN:
		if(ntohl(recvMessage->type) == AM_AVATAR_TURN) {
			// If it’s my turn to move (i.e. TurnID == AvatarID):
			if(ntohl(recvMessage->avatar_turn.TurnId) == avatarId) {
				// If my position changed:
				XYPos pos = recvMessage->avatar_turn.Pos[avatarId];
				if(pos.x != avatar->pos.x || pos.y != avatar->pos.y) {
					// Update position
					avatar->pos.x = ntohl(pos.x);
					avatar->pos.y = ntohl(pos.y);

					// Update direction of last successful move:
					dir = nextDirection;
					// check the left wall
					// if (nextDirection == M_SOUTH) {
					// 	nextDirection = M_EAST;
					// }
					// else if (nextDirection == M_EAST) {
					// 	nextDirection = M_NORTH;
					// }
					// else if (nextDirection == M_NORTH) {
					// 	nextDirection = M_WEST;
					// }
					// else if (nextDirection == M_WEST) {
					// 	nextDirection = M_SOUTH;
					// }

					i = 0;
				}
				// else { // hit a wall
				// 	// turn to the right
				// 	if (nextDirection == M_SOUTH) {
				// 		nextDirection = M_WEST;
				// 	}
				// 	else if (nextDirection == M_EAST) {
				// 		nextDirection = M_SOUTH;
				// 	}
				// 	else if (nextDirection == M_NORTH) {
				// 		nextDirection = M_EAST;
				// 	}
				// 	else if (nextDirection == M_WEST) {
				// 		nextDirection = M_NORTH;
				// 	}
				// }

				// (Note: We designate Avatar 0 as the “exit”)
				// If position == exit location (Avatar 0’s position):
				XYPos exitPos = recvMessage->avatar_turn.Pos[0];
				if(avatar->pos.x == ntohl(exitPos.x) && avatar->pos.y == ntohl(exitPos.y)) {
					nextDirection = M_NULL_MOVE;
				}
				else {
					nextDirection = (dir + (M_NUM_DIRECTIONS - 1) + i) % 4;
					nextDirection = compass[nextDirection];
				}

				// Attempt to make a move in the nextDirection
				avatarMove->type = htonl(AM_AVATAR_MOVE);
				avatarMove->avatar_move.AvatarId = htonl(avatarId);
				avatarMove->avatar_move.Direction = htonl(nextDirection);
				send(sockFd, avatarMove, sizeof(AM_Message), 0);

				i++;
			}
		} 
		else if (ntohl(recvMessage->type) == AM_MAZE_SOLVED) {// Else if AM_MAZE_SOLVED 
			if(0 == avatarId) {
				// write success message to log file
				printf("Success! Maze solved. nAvatars: %i Difficulty: %i Moves: %"PRIu32" Hash: %"PRIu32"\n", nAvatars, difficulty, ntohl(recvMessage->maze_solved.nMoves), ntohl(recvMessage->maze_solved.Hash));
			}
			// free allocated memory, etc
			fclose(logFile);
			free(avatarMove);
			free(recvMessage);
			free(avatar);
			return 0;
		}

		// free allocated memory
	}

	fclose(logFile);
	free(avatar);

	// Exit
	return 0;
}