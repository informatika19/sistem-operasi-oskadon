#ifndef _SECTOR_H_
#define _SECTOR_H_

#define INT_13H 0x13

void readSector(char *buffer, int sector);
void writeSector(char *buffer, int sector);

#endif
