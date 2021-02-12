
// KAMUS
// #define VIDEO_SEGMENT 0xB000
// #define VIDEO_BASE 0x8000
#define INT_10H 0x10


void clearScreen();
void tampil();
void handleInterrupt21 (int AX, int BX, int CX, int DX);
void interrupt(int number, int AX, int BX, int CX, int DX);
// // void printString(char *string);
// // void readString(char *string);
// void clear(char *buffer, int length); //Fungsi untuk mengisi buffer dengan 0

int main() {
      
    clearScreen();
    tampil();
    
    // makeInterrupt21();
    while (1);
}

void clearScreen() {
    interrupt(0x10,0x0000,0,0,0);

}

void tampil() {
    int ASCI_BASE = 0x0E00;
    interrupt(INT_10H, ASCI_BASE+'\n', 0, 0, 0);
    interrupt(INT_10H, ASCI_BASE+'H', 0, 0, 0);
    interrupt(INT_10H, ASCI_BASE+'e', 0, 0, 0);
    interrupt(INT_10H, ASCI_BASE+'l', 0, 0, 0);
    interrupt(INT_10H, ASCI_BASE+'l', 0, 0, 0);
    interrupt(INT_10H, ASCI_BASE+'o', 0, 0, 0);
    interrupt(INT_10H, ASCI_BASE+' ', 0, 0, 0);
    interrupt(INT_10H, ASCI_BASE+'W', 0, 0, 0);
    interrupt(INT_10H, ASCI_BASE+'o', 0, 0, 0);
    interrupt(INT_10H, ASCI_BASE+'r', 0, 0, 0);
    interrupt(INT_10H, ASCI_BASE+'l', 0, 0, 0);
    interrupt(INT_10H, ASCI_BASE+'d', 0, 0, 0);
    interrupt(INT_10H, ASCI_BASE+'!', 0, 0, 0);
}


void handleInterrupt21 (int AX, int BX, int CX, int DX){
    // switch (AX) {
    //     case 0x0:
    //         // printString(BX);
    //         break;
    //     case 0x1:
    //         // readString(BX);
    //         break;
    //     default:
    //         // printString("Invalid interrupt");
    // }
}