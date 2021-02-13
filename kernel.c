
// KAMUS
// #define VIDEO_SEGMENT 0xB000
// #define VIDEO_BASE 0x8000
#define INT_10H 0x10


void clearScreen(int mode);
void tampil();
void handleInterrupt21 (int AX, int BX, int CX, int DX);
int interrupt(int number, int AX, int BX, int CX, int DX);
void printString(char *string);
// // void readString(char *string);
// void clear(char *buffer, int length); //Fungsi untuk mengisi buffer dengan 0

int main() {
      
    clearScreen(0);
    printString("Hello World\n");
    // tampil();
    
    // makeInterrupt21();
    while (1);
}

void clearScreen(int mode) {
    /* mode 0 : default text
       mode 1 : text ART
       mode 2 : graphical
    */
    switch (mode)
    {
    case 1:
        interrupt(0x10,0x0000,0,0,0);
        break;
    case 2:
        interrupt(0x10,0x0013,0,0,0);
        break;
    default:
        interrupt(0x10,0x0003,0,0,0);
        break;
    }
}

void printString(char *string) {
    int ASCII = 0x0E00;
    int i = 0;
    while (string[i] != '\0') {
        interrupt(INT_10H, ASCII + string[i],0,0,0);
        i++;
    }
}



void handleInterrupt21 (int AX, int BX, int CX, int DX){
    switch (AX) {
        case 0x0:
            printString(BX);
            break;
        case 0x1:
            // readString(BX);
            break;
        default:
            printString("Invalid interrupt");
    }
}
