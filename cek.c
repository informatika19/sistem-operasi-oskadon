#include "lib/fileIO.h"
#include "lib/text.h"
#include "lib/sector.h"

int main() {
    int dump;
    char parentIdx[512];
    char param[512];
    char currParentIdx;

    interrupt(0x21, 0, "MASUK GAIS! \n\0",0,0);   // JIMAT, GA ADA INI ERROR
    printString("test\n");

    clear(parentIdx,512);
    clear(param,512);

    readSector(parentIdx,800);
    readSector(param,801);

    currParentIdx = parentIdx[0];
    

    // dump = findFileIdx("bin/shell",0xFF);
    // if (dump != -1) {
    //     printString("Ketemu");
    // }
    

    interrupt(0x21, 0xFF06, "bin/shell", 0x2000, &dump);
    // return 1;
}