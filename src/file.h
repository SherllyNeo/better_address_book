#ifndef _FILE
#define _FILE

#include <stdbool.h>
#include <stdio.h>
#include "shared.h"
#include "file.c"


int writeLine(Contact contact,char* filepath, int line);

int startsWithHeader(const char* filepath, const char* expectedHeader);

bool CheckIfFileExists(char *filepath);

int countLines(const char* filepath);

int deleteLine(const char* filepath, int lineToDelete);

int initBook(char* filepath);

#endif
