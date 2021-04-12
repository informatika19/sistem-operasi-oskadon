#ifndef _FILE_IO_H_
#define _FILE_IO_H_

#include "../type/boolean.h"

/** Menulis file dari buffer ke dalam sector yang kosong */
void writeFile(char *buffer, char *path, int *sectors, char parentIndex);
/** Membaca file dari sector ke dalam buffer yang kosong */
void readFile(char *buffer, char *path, int *result, char parentIndex);
/** Menghapus file */
void deleteFile(char *path, int *result, char parentIndex);

/** Fungsi penunjang */
bool isFlExist(char* dir, int parrentIdx, char* name, bool folder, int* foundIdx);
void getFlName(char* files,char filesIdx, char* name);
int foundEmptyDir(char* dir);
void writeDir(char* dir, int dirNum, int parrentIdx, int sectorIdx, char* name);

#endif
