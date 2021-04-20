#ifndef _STRING_H_
#define _STRING_H_

#include "math.h"

int strlen(char* buff);
int strcmp(char* a, char* b);
void strcpy(char* dest, char* src);
void clear(char* buffer, int length);
void printString(char* string);
void readString(char* string);

int stringLengthnoMax(char *string);
int stringLength(char *string, int max);
int stringEqual(char *s1, char *s2, int length);

#endif