#include "sector.h"

void readSector(char *buffer, int sector) {
    int AL = 0x0200;
    int num = 0x01;
    int cs = div(sector, 36) * 0x100 + mod(sector, 18) + 1;
    int hd = mod(div(sector, 18), 2) * 0x100;
    interrupt(INT_13H, AL+num, buffer, cs, hd);
}

void writeSector(char *buffer, int sector) {
    int AL = 0x0300;
    int num = 0x01;
    int cs = div(sector, 36) * 0x100 + mod(sector, 18) + 1;
    int hd = mod(div(sector, 18), 2) * 0x100;
    interrupt(INT_13H, AL+num, buffer, cs, hd);
}