#include "module/folderIO.h"
#include "module/text.h"
#include "module/sector.h"

int main() {
    int dump;
    char parentIdx[512];
    char param[512];
    int currParentIdx;

    interrupt(0x21, 0, "calling mkdir \n\0",0,0);   // Entahlah ini harus ada

    clear(parentIdx,512);
    clear(param,512);

    readSector(parentIdx,800);
    readSector(param,801);
    
    currParentIdx = parentIdx[0];
    // dump = findFileIdx("bin/shell",0xFF);
    // if (dump != -1) {
    //     printString("Ketemu");
    // }
    createFolder(param,&dump,currParentIdx);

    interrupt(0x21, 0xFF06, "bin/shell", 0x2000, &dump);
    // return 1;
}