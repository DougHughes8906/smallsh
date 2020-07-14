

#ifndef SMALLSH_H
#define SMALLSH_H

#include <string.h>

void startShell();
void action(char** argms);
void readIn(char** buffer, size_t bufSize, char** argms);

#endif
