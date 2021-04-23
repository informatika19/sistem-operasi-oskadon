#ifndef _STRING_H_
#define _STRING_H_

#include "math.h"
#include "boolean.h"

int strlen(char* buff);
int strcmp(char* a, char* b);
void strcpy(char* dest, char* src);
void clear(char* buffer, int length);
void printString(char* string);
void readString(char* string);
int ignoreSpace(char* cmd, int start);

/** Fungsi penunjang */
bool isFlExist(char* dir, int parrentIdx, char* name, bool folder, int* foundIdx);
void getFlName(char* files,char filesIdx, char* name);
int foundEmptyDir(char* dir);
void writeDir(char* dir, int dirNum, int parrentIdx, int sectorIdx, char* name);

#endif