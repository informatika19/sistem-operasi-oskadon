#include "module/text.h"

int main() {
    interrupt(0x21, 0x00, "Cek123\n", 0, 0);

    printString("Hello world\n");
}