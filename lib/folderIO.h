#ifndef _FOLDER_IO_H_
#define _FOLDER_IO_H_

#include "boolean.h"

void createFolder(char *path, int *result, char parentIndex);
void deleteFolder(char *path, int *result, char parentIndex);
void deleteFolderFlagedR(char *path, int *result, char parentIndex);
#endif