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
#include <time.h>			// for date & time in log

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
	uint32_t sockFd;
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
				fprintf(stderr, "Usage: incorrect arguments. Use --help option for usage info.\n");
				return 1;
			}
		}
		else {
			// print error to stderr
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
	if ((sockFd = ConnectToServer(serverIp, mazePort)) < 0) {
		return 1;
	}
		
	// Send AM_AVATAR_READY message
	avatarReady = calloc(1, AM_MAX_MESSAGE + 1);
	avatarReady->type = htonl(AM_AVATAR_READY);
	avatarReady->avatar_ready.AvatarId = htonl(avatarId);
	send(sockFd, avatarReady, sizeof(AM_Message), 0);
	fprintf(logFile, "Avatar ID %i sent avatar ready.\n", avatarId);

	fclose(logFile);

	// free send memory
	free(avatarReady);

	recvMessage = calloc(1, AM_MAX_MESSAGE + 1);
	avatarMove = calloc(1, AM_MAX_MESSAGE + 1);

	// While we haven’t completed the maze, listen for a message from the server:
	while( (recvMessageLen = recv(sockFd, recvMessage, sizeof(AM_Message), 0) > 0) 	) {
		// open/close log file each iteration of the loop so that avatars don't print out of order
		logFile = fopen(logFilePath, "a");
		// evaluate message type
		if (IS_AM_ERROR(ntohl(recvMessage->type))){
			if(0 == avatarId) {
				fprintf(logFile, "Error %"PRIu32": ", ntohl(recvMessage->type));

				if (ntohl(recvMessage->type) == AM_SERVER_OUT_OF_MEM) {
					fprintf(logFile, "server out of memory.\n");
				}
				else if (ntohl(recvMessage->type) == AM_SERVER_DISK_QUOTA) {
					fprintf(logFile, "server disk quota exceeded.\n");
				}
				else if (ntohl(recvMessage->type) == AM_UNEXPECTED_MSG_TYPE) {
					fprintf(logFile, "unexpected message type.\n");
				}
				else if (ntohl(recvMessage->type) == AM_TOO_MANY_MOVES) {
					fprintf(logFile, "too many moves.\n");
				}
				else if (ntohl(recvMessage->type) == AM_UNKNOWN_MSG_TYPE) {
					fprintf(logFile, "unknown message type %i.\n", ntohl(recvMessage->unknown_msg_type.BadType));
				}
			}

			if (ntohl(recvMessage->type) == AM_NO_SUCH_AVATAR) {
				fprintf(logFile, "Error: no avatar with ID %i\n", avatarId);
			}
			else if (ntohl(recvMessage->type) == AM_AVATAR_OUT_OF_TURN) {
				fprintf(logFile, "Error: avatar %i out of turn.\n", avatarId);
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
				// log the turn data
				fprintf(logFile, "received msg: AM_AVATAR_TURN (TurnId: %"PRIu32, ntohl(recvMessage->avatar_turn.TurnId));
				for (int id = 0; id < AM_MAX_AVATAR; id++) {
					fprintf(logFile, ", a%i = (%i,%i)", id, (unsigned int) ntohl(recvMessage->avatar_turn.Pos[id].x), (unsigned int) ntohl(recvMessage->avatar_turn.Pos[id].y));
				}
				fprintf(logFile, ")\n");

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

				fprintf(logFile, "sent msg: AM_AVATAR_MOVE (AvatarId = %i, Direction = %i)\n", avatarId, nextDirection);
				i++;
			}
		} 
		else if (ntohl(recvMessage->type) == AM_MAZE_SOLVED) {
			if(0 == avatarId) {
				time_t now;
				time(&now);
				fprintf(logFile, "Success! Maze solved. nAvatars: %i Difficulty: %i Moves: %"PRIu32" Hash: %"PRIu32" %s", nAvatars, difficulty, ntohl(recvMessage->maze_solved.nMoves), ntohl(recvMessage->maze_solved.Hash), ctime(&now));			
			}
			// free allocated memory, etc
			fclose(logFile);
			free(avatarMove);
			free(recvMessage);
			free(avatar);
			return 0;
		}
		fclose(logFile);
	}

	free(avatar);
	return 1;
}