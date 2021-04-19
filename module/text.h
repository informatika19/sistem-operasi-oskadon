#ifndef _TEXT_H_
#define _TEXT_H_

#include "math.h"

void printString(char *string);
void readString(char *string);
void clear(char *buffer, int length);
int stringLengthnoMax(char *string);
int stringLength(char *string, int max);
int stringEqual(char *s1, char *s2, int length);

#endif