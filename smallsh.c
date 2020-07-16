
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

// kills all processes/jobs that the shell started
void exitShell() {
	return;
}

// checks to see what background processes (if any) have terminated
// and if so, clean them up and print a message to the shell
void bkgCleanup(int* bkgProcesses, int* bkgArrSize, int* numBkgs) {
	return;
}

// executes any command that is not built in to the shell
void executeOther(int maxStrLen, int numArgs, char** argms, 
	char command[maxStrLen], char inputFile[maxStrLen], 
	char outputFile[maxStrLen], int bkgFlag, int bkgOn, int* bkgProcesses,
	int* bkgArrSize, int* numBkgs, int* exitStatus, int* exitLast) {

	// check to see if this is supposed to be a background process
	// and if background processes are currently allowed 
	if (bkgFlag && bkgOn) {

	}

	// this will be executed as a foreground process
	else {

	}
}


// execute a built-in command, this could only be cd or status
void executeBuiltin(int maxStrLen, int numArgs, char** argms, 
	char command[maxStrLen], int* exitStatus, int lastSignal, int* exitLast) {
	// see if this si a cd command
	if (strcmp(command, "cd") == 0) {
		// value returned by the chdir function
		// -1 for error
		// 0 for success
		int exitVal;
		// no arguments entered, so this command should bring the user
		// to the home directory
		if (numArgs == 0) {
			// get the name of the home directory
			char* homeDir = getenv("HOME");
			exitVal = chdir(homeDir);
		}

		// the first argument is passed as the path of the directory to 
		// change to, all other arguments are ignored
		else {
			exitVal = chdir(argms[0]);
		}

		// show the current working directory after using chdir
		char curWorkDir[1000];
		getcwd(curWorkDir, sizeof(curWorkDir));
		printf("Working directory after call to cd: %s\n", curWorkDir);

	}
	
	// the only other possible command is status
	else {
		// the last non builtin foreground process exited normally
		if (*exitLast) {
			printf("exit value %d\n", *exitStatus);
		}
		// the last non builtin foreground was terminated with a signal
		else {
			printf("terminated by signal %d\n", lastSignal);
		}
	}
}

// actions the given commands
void action(int maxStrLen, int numArgs, char** argms, char command[maxStrLen],
	char inputFile[maxStrLen], char outputFile[maxStrLen], int bkgFlag, 
	int bkgOn, int* bkgProcesses, int* bkgArrSize, int* numBkgs, 
	int* exitStatus, int lastSignal, int* exitLast) {

 	printf("Command entered: %s\n", command);

	printf("Command arguments:\n");
	for (int i = 0; i < numArgs; i++) {
		printf("%s\n", argms[i]);
	}

	printf("Input file: %s\n", inputFile);
	printf("Output file: %s\n", outputFile);

	printf("bkgFlag: %d\n", bkgFlag);

	// see if this a built-in command (besides exit which would have
	// already been handled by the runShell function)
	if (strcmp(command, "cd") == 0 || strcmp(command, "status") == 0) {
		// built-in commands will ignore output/input redirection and
		// will always be run in the foreground
		executeBuiltin(maxStrLen, numArgs, argms, command, exitStatus, lastSignal,
			exitLast);
	}
	// this is not a built-in command
	else {
		executeOther(maxStrLen, numArgs, argms, command, inputFile, outputFile,
			bkgFlag, bkgOn, bkgProcesses, bkgArrSize, numBkgs, exitStatus, exitLast);
	}
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

	// check to see if the word is $$, in which case it should be
	// changed to pid of the shell process
	if (strcmp(wordBuf, "$$") == 0) {
		pid_t curPID = getpid();
		char pidStr[maxStrLen];
		sprintf(pidStr, "%d", curPID);
		strcpy(wordBuf, pidStr);
	}
	
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
	// set to 1 when background processes are allowed and 0 if background
	// processes are not allowed. This can be toggled by the user with a
	// CTRL-Z command (SIGSTP signal)
	int bkgOn = 1;
	// holds the current size of the bkgProcesses array
	int bkgArrSize = 5;
	// an integer array that will hold the PIDs of all running background 
	// processes
	int* bkgProcesses = malloc(bkgArrSize * sizeof(int));
	// holds the number of currently runnign background processes
	int numBkgs = 0;
	// holds the exit status of the last non-builtin foreground command
	int exitStatus = 0;
	// holds the signal code of the last terminating signal
	int lastSignal = -1;
	// holds 1 if the last non builtin foreground process terminted 
	// normally and 0 if it was terminated with a signal
	int exitLast = 1;

 
  while (strcmp(command, exitStr) != 0) {

    // action the last issued command (which can't be exit)
		action(MAX_LEN, numArgs, argms, command, inputFile, outputFile, bkgFlag,
			bkgOn, bkgProcesses, &bkgArrSize, &numBkgs, &exitStatus, lastSignal, 
			&exitLast);

		// clean up background processes
		bkgCleanup(bkgProcesses, &bkgArrSize, &numBkgs);

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

	printf("exit shell function about to be entered\n");
	// the user has decided to exit the shell, kill all processes and/or 
	// jobs the shell has started and exit
	exitShell();

  // free memory allocated for the command arguments
  for (int i = 0; i < NUM_ARGS; i++) {
    free(argms[i]);
    argms[i] = NULL;
  }
  free(argms);
	argms = NULL;

	// free memory allocated for the background process ids
	free(bkgProcesses);
	bkgProcesses = NULL;
}
