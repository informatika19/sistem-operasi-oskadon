
// KAMUS
#define INT_10H 0x10
#define INT_16H 0x16
#define VIDEO_SEGMENT 0xB000
#define BASE  0x8000
#define SCREEN_SIZE 25*80*2
int nul = 0x0;      // null (\0)
int bs = 0x8;       // backspace (\b)
int ht = 0x9;       // horizontal tab (\t)
int lf = 0xa;       // line feed (\n) 
int vt = 0xb;       // vertical tab (\v)
int ff = 0xc;       // form feed (\f)
int cr = 0xd;       // carriage return (\r) (enter)
int esc = 0x1b;     // escape (\e)

// Graphic
int max_X = 320;
int max_Y = 200;
// int mode;

extern char imageFile;


void putInMemory (int segment, int address, char character);
int modeScreen(int mode);

void handleInterrupt21 (int AX, int BX, int CX, int DX);
int interrupt(int number, int AX, int BX, int CX, int DX);

void printString(char *string);
void readString(char *string);
void clear(char* buffer, int length);       //Fungsi untuk mengisi buffer dengan 0
int mod(int dividend, int divisor);         // long apa int?
int div(int numerator, int denominator);    // long apa int?

void initDrawImage();
void drawImage();
void drawSquare();
void drawSomething();


int main() {
    char buff[256];

    int dump = modeScreen(2);

    
    drawSomething();
    dump = modeScreen(0);
    printString("Hello World\n");
    //buff = "Hello World\n";
     readString(buff);
     printString(buff);

    
    while (1);
}

int modeScreen(int mode) {
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
    return;
}
void printString( char* s){
    putInMemory(0xB000, 0x8000, 'H');
    putInMemory(0xB000, 0x8001, 0xD);
    putInMemory(0xB000, 0x8002, 'a');
    putInMemory(0xB000, 0x8003, 0xD);
    putInMemory(0xB000, 0x8004, 'i');
    putInMemory(0xB000, 0x8005, 0xD);


/*
   int i=0;
   int row = 0;
   int col = 0;
   int color= 0; 
   int bg = 0;
   int offset=BASE+((row-1)*80*2)+col*2;
   while(s[i]!='\0'){
      putInMemory(VIDEO_SEGMENT, offset + i*2, s[i]);
      putInMemory(VIDEO_SEGMENT, offset + i*2+1, color + (bg<<4));
      i++;
   }*/
}

// void printString(char *string) {
//     int AL = 0x0E00;
//     int i = 0;
//     while (string[i] != '\0') {
//         interrupt(INT_10H, AL + string[i],0,0,0);

//         if (string[i] == '\n') {
//             // interrupt(INT_10H,AL+lf,0,0,0);
//             interrupt(INT_10H,AL+cr,0,0,0);
//         }
//         i++;
//     }
    
    
// }


void readString(char* string) {    
    int AL = 0x0E00;
    int loop = 1;
    int i = 0;
    int ascii;

    while (loop)
    {
        ascii = interrupt(INT_16H,0,0,0,0);
        if (ascii == cr) {
            string[i] = nul;
            interrupt(INT_10H,AL+lf,0,0,0);
            interrupt(INT_10H,AL+cr,0,0,0);
            loop = 0;
        } else if (ascii == bs) {
            if (i > 0) {
                interrupt(INT_10H,AL+bs,0,0,0);
                interrupt(INT_10H,AL+nul,0,0,0);
                interrupt(INT_10H,AL+bs,0,0,0);
                i--;
            }
        } else {
            string[i] = ascii;
            interrupt(INT_10H,AL+ascii,0,0,0);
            i++;
        }
         
    }
    
}

void drawSomething() {
    char* image = &imageFile;
    int address;

    int x_size = image[0];
    int y_size = image[1];

    
    // Coba bikin ketengah
    int halfDif_x = div((max_X - x_size),2);
    int halfDif_y = div((max_Y - y_size),2);

    int x = halfDif_x;
    int y = halfDif_y;
    
    int i = 2;

    while (y < y_size + halfDif_y) {
        while (x < x_size + halfDif_x) {
            address = 320*y + x;
            putInMemory(0xA000,address,image[i]);
            i++;
            x++;
        }
        x = halfDif_x;
        y++;
    }
    // return;
}


void drawSquare() {
    int x = 0;
    int y = 0;
    int AL = 0x0C00;
    
    while (y < max_Y) {
        while (x < max_X){
            if ((x >= 100 && x <= 150) && (y >= 50 && y <= 100)) {
                interrupt(INT_10H,AL + 14,0,x,y);
            }

            
            x++;
        }
        x=0;
        y++;
    }   
}




void handleInterrupt21 (int AX, int BX, int CX, int DX){
    switch (AX) {
        case 0x0:
            printString(BX);
            break;
        case 0x1:
            readString(BX);
            break;
        default:
            printString("Invalid interrupt");
    }
}

// clear
void clear(char* buffer, int length){
    int i = 0;
    while (i < length) {
        buffer[i] = 0x0;
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