#include "module/fileIO.h"
#include "module/text.h"
#include "module/sector.h"

int main() {
    int dump;
    char parentIdx[512];
    char param[512];

    clear(parentIdx,512);
    clear(param,512);


    interrupt(0x21, 0, "Masuk cok! : \0",0,0);
    printString("test\n");


    readSector(parentIdx,800);
    readSector(param,801);

    printString("test param\n");
    printString(param);
    printString("\n");
    

    // dump = findFileIdx("shell",0xFF);
    // if (dump != -1) {
    //     printString("Ketemu");
    // }
    
    

    interrupt(0x21, 0xFF06, "bin/shell", 0x2000, &dump);
    // return 1;
}