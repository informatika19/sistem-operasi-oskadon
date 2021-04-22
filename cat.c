#include "module/boolean.h"
#include "module/fileIO.h"
#include "module/sector.h"

int main() {
    int dump;
    char parentIdx[512];
    char param[512];
    char currParentIdx;

    char buffer[512*16];
    int result;
    int tmp;

    interrupt(0x21, 0, "Executing cat...\n\0",0,0);   // Entahlah ini harus ada

    clear(parentIdx,512);
    clear(param,512);

    readSector(parentIdx,800);
    readSector(param,801);

    currParentIdx = parentIdx[0];

    
    readFile(buffer, param, &result, currParentIdx);
    // printString("done reading file\n");
    if (result == -1) {
        printString("ERROR : File tidak ditemukan\n");
        interrupt(0x21, 0xFF06, "bin/shell", 0x2000, &dump);
    }else{
        printString(buffer);
        printString("\n");
    }
      

    interrupt(0x21, 0xFF06, "bin/shell", 0x2000, &dump);
    // return 1;
}