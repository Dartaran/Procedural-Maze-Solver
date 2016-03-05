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