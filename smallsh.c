
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


// actions the given commands
void action(int maxStrLen, int numArgs, char** argms, char command[maxStrLen],
	char inputFile[maxStrLen], char outputFile[maxStrLen], int bkgFlag) {

 	printf("Command entered: %s\n", command);

	printf("Command arguments:\n");
	for (int i = 0; i < numArgs; i++) {
		printf("%s\n", argms[i]);
	}

	printf("Input file: %s\n", inputFile);
	printf("Output file: %s\n", outputFile);

	printf("bkgFlag: %d\n", bkgFlag);
}


// reads the next word from a buffer into the wordBuf array. Also tracks
// and updates the current index in the buffer (curBufInd). If the end of 
// the line being read in is reached, -1 is returned. Otherwise 0 is 
// returned
int readWord(char* buffer, size_t bufSize, int *curBufInd, int maxStrLen, 
	char wordBuf[maxStrLen])  {

	int curWordInd = 0;
	
	// read in the next word to wordBuf
	while (*curBufInd < bufSize && curWordInd < maxStrLen && 
		buffer[*curBufInd] != ' ' && buffer[*curBufInd] != '\t' &&
		buffer[*curBufInd] != '\n') {
		
		wordBuf[curWordInd] = buffer[*curBufInd];
		curWordInd++;
		(*curBufInd)++;	
	}

	// set the last character for the word as the null terminator
	wordBuf[curWordInd] = '\0';
	
	// clear out the whitespace until you hit a new word or the end of
	// the line
	while (*curBufInd < bufSize && (buffer[*curBufInd] == ' ' ||
		buffer[*curBufInd] == '\t')) {
		(*curBufInd)++;		
	}	

	// check to see if the end of the line has been reached
	if (*curBufInd == bufSize || buffer[*curBufInd] == '\n') {
		return -1;
	}
	// there is at least one more word on the line, return 0
	return 0;
}
			
// reads in a command and stores its arguments in argms
// if there is input redirection, the name of the file to redirect to
// is stored in input file. If there is output redirection, the name of the
// file to redirect to is stored in outputFile. Finally, if the command
// is meant to be executed in the background, the bkgFlag variable is set to
// 1 (0 otherwise). Returns the number of command arguments (i.e. the 
// number of arguments finally stored in argms, so this does not count
// output/input redirection or the ending & if there is one)
int readIn(char* buffer, size_t bufSize, int maxStrLen, 
	char command[maxStrLen], char** argms, char inputFile[maxStrLen],
	char outputFile[maxStrLen], int* bkgFlag) {	

	// reset the bkgFlag to 0
	*bkgFlag = 0;
	// reset the input file and output file to empty strings
	strcpy(outputFile, "");
	strcpy(inputFile, "");

  // get to the first non-empty space character
  int curInd = 0;
	
  while (curInd < (int) bufSize && (buffer[curInd] == ' ' ||
    buffer[curInd] == '\t')) {	
    curInd++;
  }	
	
  // if we've reached the end of the line, or a comment character,
  // no command was entered, so the command string is updated to blank
  if (curInd == bufSize || buffer[curInd] == '\n' ||
    buffer[curInd] == '#') {
 		strcpy(command, "blank");   
    printf("Blank or a comment was entered\n");
		// nothing further needs to be updated given that all  other command
		// variables will be ignored when no command is entered. Indicate 
		// that there are 0 command arguments
		return 0;
  }

  else {
    printf("A command that needs to be processed was entered.\n");

   	// the first word on the line is the command, read it in and save it
   	// to command
		int readStatus = readWord(buffer, bufSize, &curInd, maxStrLen, command);   	
		// index of the current element of argms that will store the next
		// word in the command
		int argInd = 0;

		// tracks the number of arguments entered
		int numArgs = 0;
	
		// if the readStatus is -1, then there are no more words on the line
		while (readStatus != -1) {
			readStatus = readWord(buffer, bufSize, &curInd, maxStrLen, argms[argInd]);
		
			numArgs++;
			argInd++;
		}	

		// check to see if the last argument read in was &, indicating this
		// process is meant to be executed in the background
		if (numArgs >= 1 && strcmp(argms[numArgs - 1], "&") == 0) {
			// store this info with the flag rather than keeping this as an
			// argument	
			*bkgFlag = 1;
			// remove from argms
			strcpy(argms[numArgs - 1], "");
			numArgs--;	
		}	

		// check to see if there is either output redirection or input
		// redirection
		if (numArgs >= 2 && strcmp(argms[numArgs - 2], ">") == 0) {
			// there is output redirection, save the file name
			strcpy(outputFile, argms[numArgs - 1]);
			// remove the last two arguments from argms given this info
			// is now stored in outputFile
			strcpy(argms[numArgs - 1], "");
			strcpy(argms[numArgs - 2], "");
			numArgs -= 2;
	
			// check to see if there is also input redirection
			if (numArgs >= 2 && strcmp(argms[numArgs - 2], "<") == 0) {
				// there is input redirection, save the file name
				strcpy(inputFile, argms[numArgs - 1]);
				// remove the last two arguments from argms given this info
				// is now stored in inputFile
				strcpy(argms[numArgs - 1], "");
				strcpy(argms[numArgs - 2], "");
				numArgs -= 2;
			}
		}

		else if (numArgs >= 2 && strcmp(argms[numArgs - 2], "<") == 0) {
			// there is input redirection, save the file name
			strcpy(inputFile, argms[numArgs - 1]);
			// remove the last two arguments from argms given this info
			// is now stored in inputFile
			strcpy(argms[numArgs - 1], "");
			strcpy(argms[numArgs - 2], "");
			numArgs -= 2;

			// check to see if there is also output redirection
			if (numArgs >= 2 && strcmp(argms[numArgs - 2], ">") == 0) {
				// there is output redirection, save the file name
				strcpy(outputFile, argms[numArgs - 1]);
				// remove the last two arguments from argms given this info
				// is now stored in outputFile
				strcpy(argms[numArgs - 1], "");
				strcpy(argms[numArgs - 2], "");
				numArgs -= 2;
			}
		}

		return numArgs; 
  }
}


// starts the shell and enters a loop that takes in commands until
// the user wants to exit
void runShell() {

  const int NUM_ARGS = 512;
  const int MAX_LEN = 2048;

  // allocate space to hold the command arguments 
  char** argms = (char**) malloc(NUM_ARGS * sizeof(char*));

  for (int i = 0; i < NUM_ARGS; i++) {
    argms[i] = malloc(MAX_LEN * sizeof(char)); 
  } 

  // string that will be used to test if the user wants to exit
  char exitStr[] = "exit";

	char command[MAX_LEN];
	// set the initial command to blank which will do nothing
	strcpy(command, "blank");

	// the name of the input file, if there is input redirection.
	// If no input redirection, it is an empty string
	char inputFile[MAX_LEN];
	strcpy(inputFile, "");

	// name of the output file if there is output redirection
	char outputFile[MAX_LEN];
	strcpy(outputFile, "");

	// set to 1 if the command is meant to be processed in the background
	// and 0 otherwise
	int bkgFlag = 0;
	// keeps track of the number of arguments in the latest command. Does not
	// count output/input redirection or the final & if there is one
	int numArgs = 0;
 
  while (strcmp(command, exitStr) != 0) {

    // action the last issued command (which can't be exit)
		action(MAX_LEN, numArgs, argms, command, inputFile, outputFile, bkgFlag);
    printf(": ");
		fflush(stdout);

    char* buffer = NULL;
    size_t bufSize = 0;
    getline(&buffer, &bufSize, stdin);
	
    // read in the command and set the various commmand variables
    // accordingly
    numArgs = readIn(buffer, bufSize, MAX_LEN, command, argms, inputFile, 
			outputFile, &bkgFlag);

    // free the memory allocated for the buffer
    free(buffer); 
  }

  // free memory allocated for the command arguments
  for (int i = 0; i < NUM_ARGS; i++) {
    free(argms[i]);
    argms[i] = NULL;
  }
  free(argms);
}
