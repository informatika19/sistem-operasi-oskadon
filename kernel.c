
// KAMUS
#define INT_10H 0x10


void clearScreen(int mode);
//void tampil();
void handleInterrupt21 (int AX, int BX, int CX, int DX);
int interrupt(int number, int AX, int BX, int CX, int DX);
void printString(char *string);
// void readString(char *string);
void clear(char *buffer, int length);       //Fungsi untuk mengisi buffer dengan 0
int mod(int dividend, int divisor);         // long apa int?
int div(int numerator, int denominator);    // long apa int?

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

// clear
void clear(char* buffer, int length){
    for (int i = 0; i < length; i++){
        buffer[i] = 0x0;    // isi dengan 0
    }
}

// modulo
int mod(int dividend, int divisor){
    while (dividend >= divisor){
        dividend -= divisor;
    }
    return dividend;
}

// division
int div(int numerator, int denominator){
    int res = 0;
    while (numerator >= denominator * res){
        res += 1;
    }
    res -= 1;
    return res;
}