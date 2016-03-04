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
#define HELPFILE "../help/amazing_help.txt"

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

	unsigned int i, dir, nextDirection;
	struct Avatar* avatar;
	struct AM_Message* avatarReady;
	struct AM_Message* avatarMove;
	ssize_t recvMessageLen;				// in bytes
	AM_Message* recvMessage;
	
	// Check args (checking number of args is sufficient)
	if( 7 != argc) {
		if (2 == argc) {
			if ( 0 == strcmp("--help", argv[1]) ){
				DisplayFile(HELPFILE);
				return 0;
			}
			else {
				printf("Usage: incorrect arguments. Use --help option for usage info.\n");
				return 1;
			}
		}
		else {
			// print error to fprintf
			fprintf(stderr, "Incorrect argument count for avatar %i\n", atoi(argv[2]));
			exit (1);
		}
	}

	printf("test\n");

	// Initialize local variables
	avatarId = atoi(argv[1]);
	nAvatars = atoi(argv[2]);
	difficulty = atoi(argv[3]);
	serverIp = argv[4];
	mazePort = atoi(argv[5]);
	logFilePath = malloc(sizeof(argv[6]) + 1);
	strcpy(logFilePath, argv[6]);
	logFile = fopen(logFilePath, "a");

	dir = M_NORTH;				// direction of last successful move
	nextDirection = M_WEST;		// direction to attempt next
	i = 0;						// a count of unsuccessful moves, such that 
								// 0 indicates a left turn; 1 ahead; 2 right; 3 back

	avatar = malloc( sizeof(Avatar) );
	avatar->fd = avatarId;
	avatar->pos.x = -1;		// initialized to impossible value
	avatar->pos.y = -1;

	if ((sockFd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(logFile, "Error: failed to create the socket\n");
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
		fprintf(logFile, "Error: could not connect to the server: %s.\n", strerror(errno));
		return 1;
	}

	// Send AM_AVATAR_READY message
	avatarReady = calloc(1, AM_MAX_MESSAGE + 1);
	avatarReady->type = htons(AM_AVATAR_READY);
	avatarReady->avatar_ready.AvatarId = htons(avatarId);
	send(sockFd, avatarReady, sizeof(AM_Message), 0);
	fprintf(logFile, "Avatar ID %i sent avatar ready.\n", avatarId);

	// free send memory
	free(avatarReady);

	recvMessage = calloc(1, AM_MAX_MESSAGE + 1);
	avatarMove = calloc(1, AM_MAX_MESSAGE + 1);
	// While we haven’t completed the maze, listen for a message from the server:
	while( (recvMessageLen = recv(sockFd, recvMessage, AM_MAX_MESSAGE, 0) > 0) 	) {
		//TODO Evaluate message type
		if (IS_AM_ERROR(recvMessage->type)){
			fprintf(logFile, "Error: ");
			// AM_SERVER_OUT_OF_MEM
			if(0 == avatarId) {
				if (ntohl(recvMessage->type) == AM_SERVER_OUT_OF_MEM) {
					fprintf(logFile, "server out of memory.\n");
					return 1;
				}
				else if (ntohl(recvMessage->type) == AM_SERVER_DISK_QUOTA) {
					fprintf(logFile, "server disk quota exceeded.\n");
					return 1;
				}
				else if (ntohl(recvMessage->type) == AM_UNEXPECTED_MSG_TYPE) {
					fprintf(logFile, "unexpected message type.\n");
				}
				else if (ntohl(recvMessage->type) == AM_UNKNOWN_MSG_TYPE) {
					fprintf(logFile, "unknown message type %i.\n", ntohl(recvMessage->unknown_msg_type.BadType));
				}
			}

			if (ntohl(recvMessage->type) == AM_NO_SUCH_AVATAR) {
				fprintf(logFile, "no avatar with ID %i\n", avatarId);
				return 1;
			}
			else if (ntohl(recvMessage->type) == AM_AVATAR_OUT_OF_TURN) {
				fprintf(logFile, "avatar %i out of turn.\n", avatarId);
			}

			//TODO free allocated memory, etc
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
					i = 0;
				}

				// (Note: We designate Avatar 0 as the “exit”)
				// If position == exit location (Avatar 0’s position):
				if(avatar->pos.x == ntohl(pos.x) && avatar->pos.y == ntohl(pos.y)) {
					nextDirection = M_NULL_MOVE;
				}
				else {
					nextDirection = (dir + (M_NUM_DIRECTIONS - 1) + i) % 4;
				}

				// Attempt to make a move in the nextDirection
				avatarMove->avatar_move.AvatarId = htons(avatarId);
				avatarMove->avatar_move.Direction = htons(nextDirection);
				send( sockFd, avatarMove, htons( sizeof(AM_Message) ), 0);

				i++;
			}
		} 
		else if (ntohl(recvMessage->type) == AM_MAZE_SOLVED) {// Else if AM_MAZE_SOLVED 
			if(0 == avatarId) {
				// write success message to log file
				fprintf(logFile, "Success! Maze solved. nAvatars: %i Difficulty: %i Moves: %"PRIu32" Hash: %"PRIu32"\n", nAvatars, difficulty, ntohl(recvMessage->maze_solved.nMoves), ntohl(recvMessage->maze_solved.Hash));
			}
			// free allocated memory, etc
			fclose(logFile);
			free(avatarMove);
			free(recvMessage);
			free(avatar);
			return 0;
		}

		// free allocated memory
		free(recvMessage);
	}

	free(avatar);

	// Exit
	return 0;
}