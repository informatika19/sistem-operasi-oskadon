#include "module/text.h"
#include "module/sector.h"

int main() {
    int dump;
    interrupt(0x21, 0, "Gaada cok! : \0",0,0);
    printString("test\n");
    

    interrupt(0x21, 0xFF06, "shell", 0x2000, &dump);
    // return 1;
}