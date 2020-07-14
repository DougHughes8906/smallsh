
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// actions the given commands
void action(char** argms) {

  // argument value to signal the command has ended
  char endArg[] = " ";

  // check to see if a blank line or comment was entered
  // in which case do nothing
  if (strcmp(argms[0], endArg) != 0) {
    printf("Neither a blank line nor a comment was entered\n");
  }
}

// reads in a command and stores its arguments in argms
void readIn(char* buffer, size_t bufSize, char** argms) {	

  // get to the first non-empty space character
  int curInd = 0;
	
  while (curInd < (int) bufSize && (buffer[curInd] == ' ' ||
    buffer[curInd] == '\t')) {	
    curInd++;
  }	

	printf("curIndex: %d\n", curInd);
	printf("cur char: %c\n", buffer[curInd]);

  // if we've reached the end of the line, or a comment character,
  // we should reprompt (done by sending a blank space as the first
  // argument to the action function
  if (curInd == bufSize || buffer[curInd] == '\n' ||
    buffer[curInd] == '#') {
    argms[0][0] = ' ';
    argms[0][1] = '\0';
    printf("Blank or a comment was entered\n");
  }

  else {
    printf("A command that needs to be processed was entered.\n");

    // see if this command is exit
    int argInd = 0;
    argms[0][argInd] = buffer[curInd];
    curInd++;
    argInd++;
    while (curInd < bufSize && buffer[curInd] != ' ' &&
      buffer[curInd] != '\t' && buffer[curInd] != '\0'
			&& buffer[curInd] != '\n') {
      argms[0][argInd] = buffer[curInd];
      curInd++;
      argInd++; 
    }
    argms[0][argInd] = '\0';
  }
}


// starts the shell and enters a loop that takes in commands until
// the user wants to exit
void startShell() {

  const int NUM_ARGS = 512;
  const int MAX_LEN = 2048;

  // allocate space to hold the command arguments 
  char** argms = (char**) malloc(NUM_ARGS * sizeof(char*));

  for (int i = 0; i < NUM_ARGS; i++) {
    argms[i] = malloc(MAX_LEN * sizeof(char));
    // set all arguments to be the blank line/ comment argument
    argms[i][0] = ' ';
    argms[i][1] = '\0';
  } 

  // string that will be used to test if the user wants to exit
  char exitStr[] = "exit";
 
  while (strcmp(argms[0], exitStr) != 0) {

    // action the last issued command (which can't be exit)
    action(argms);

    printf(": ");
    char* buffer = NULL;
    size_t bufSize = 0;
    getline(&buffer, &bufSize, stdin);
	
    // read in the command to argms 
    readIn(buffer, bufSize, argms);

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
