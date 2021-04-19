#ifndef _FILE_IO_H_
#define _FILE_IO_H_

#include "math.h"
#include "text.h"

void readFile(char *buffer, char *path, int *result, char parentIndex);
void writeFile(char *buffer, char *path, int *sectors, char parentIndex);
void readSector(char *buffer, int sector);
void writeSector(char *buffer, int sector);

#endif