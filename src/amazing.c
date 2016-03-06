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
	int i, dir, nextDirection, nextCompassIndex;
	unsigned int avatarId, nAvatars, difficulty, mazePort;
	uint32_t sockFd = 0;
	char* serverIp;
	char* logFilePath;
	FILE* logFile;
	struct Avatar* avatar;
	struct AM_Message* avatarReady;
	struct AM_Message* avatarMove;
	ssize_t recvMessageLen;				// in bytes
	AM_Message* recvMessage;
	const int compass[] = {M_WEST, M_NORTH, M_EAST, M_SOUTH};
	
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
			fprintf(stderr, "Incorrect argument count for avatar %i.  Usage ./amazing [id] [nAvatars] [difficulty] [ip] [mazePort] [fileName]\n", atoi(argv[2]));	
			exit (1);
		}
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

	dir = 1;					// direction of last successful move
	nextCompassIndex = 0;
	nextDirection = compass[nextCompassIndex];		// direction to attempt next
	i = 0;						// a count of unsuccessful moves, such that 
								// 0 indicates a left turn; 1 ahead; 2 right; 3 back
	avatar = malloc( sizeof(Avatar) );
	avatar->fd = avatarId;
	avatar->pos.x = -1;			// initialized to impossible value
	avatar->pos.y = -1;

	// attempt to connect to the server
	if (!ConnectToServer(sockFd, serverIp, mazePort)) {
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

		else if(ntohl(recvMessage->type) == AM_AVATAR_TURN) {
			// If it’s my turn to move:
			if(ntohl(recvMessage->avatar_turn.TurnId) == avatarId) {
				// If my position changed:
				XYPos pos = recvMessage->avatar_turn.Pos[avatarId];
				if(ntohl(pos.x) != avatar->pos.x || ntohl(pos.y) != avatar->pos.y) {
					// Update position
					avatar->pos.x = ntohl(pos.x);
					avatar->pos.y = ntohl(pos.y);

					// Update direction of last successful move:
					dir = nextCompassIndex;

					i = 0;
				}

				// If I have reached the exit location (we designate Avatar 0 as the “exit”):
				XYPos exitPos = recvMessage->avatar_turn.Pos[0];
				if(avatar->pos.x == ntohl(exitPos.x) && avatar->pos.y == ntohl(exitPos.y)) {
					nextDirection = M_NULL_MOVE;
				}
				else {
					nextCompassIndex = (dir + (M_NUM_DIRECTIONS - 1) + i) % 4;
					nextDirection = compass[nextCompassIndex];
				}

				// Attempt to make a move in the nextDirection
				avatarMove->type = htonl(AM_AVATAR_MOVE);
				avatarMove->avatar_move.AvatarId = htonl(avatarId);
				avatarMove->avatar_move.Direction = htonl(nextDirection);
				send(sockFd, avatarMove, sizeof(AM_Message), 0);

				i++;
			}
		} 

		else if (ntohl(recvMessage->type) == AM_MAZE_SOLVED) {
			if(0 == avatarId) {
				printf("Success! Maze solved. nAvatars: %i Difficulty: %i Moves: %"PRIu32" Hash: %"PRIu32"\n", nAvatars, difficulty, ntohl(recvMessage->maze_solved.nMoves), ntohl(recvMessage->maze_solved.Hash));
			}
			// free allocated memory, etc
			fclose(logFile);
			free(avatarMove);
			free(recvMessage);
			free(avatar);
			return 0;
		}
	}

	fclose(logFile);
	free(avatar);
	return 1;
}