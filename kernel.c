// Library
#include "module/boolean.h"
#include "module/fileIO.h"
#include "module/folderIO.h"
#include "module/math.h"
#include "module/text.h"
#include "module/sector.h"


// Fungsi Bawaan
void putInMemory (int segment, int address, char character);
void handleInterrupt21 (int AX, int BX, int CX, int DX);
int interrupt(int number, int AX, int BX, int CX, int DX);

// Fungsi Graphic dan Hiasan
int modeScreen(int mode);
void biosLogo();
void asciiART();


int main() {
    char buff[1024];    // Buff untuk menyimpan banyaknya character dari pengguna
    int dump;

    // Tampilkan tampilan awal bios dengan graphic
    dump = modeScreen(2);    // Ganti mode menjadi graph mode (Sekalian clear screen)
    biosLogo();
    interrupt(0x15,0x8600,0,30,0); // wait CK:100 = 1000 ms

    // Tampilkan tampilan awal bios dengan ASCII ART
    dump = modeScreen(0);    // Ganti mode menjadi text mode (Sekalian clear screen)
    asciiART();
    

    // Say Hello To World!
    handleInterrupt21 (0, "Hello World\n", 0, 0);
    // shell();


    while (1) {
        // Loop selamanya untuk meminta input string dari user dan menampilkannya pada layar
        readString(buff);
        printString(buff);
        printString("\n");
    };
}


// Membuat Interupt21 (DOS Interupt) untuk menjalankan perintah dari pengguna
void handleInterrupt21 (int AX, int BX, int CX, int DX) { 
    char AL, AH; 
    AL = (char) (AX); 
    AH = (char) (AX >> 8); 
    switch (AL) { 
        case 0x00: 
            printString(BX); 
            break; 
        case 0x01: 
            readString(BX); 
            break; 
        case 0x02: 
            readSector(BX, CX); 
            break; 
        case 0x03: 
            writeSector(BX, CX); 
            break; 
        case 0x04: 
            readFile(BX, CX, DX, AH); 
            break; 
        case 0x05: 
            writeFile(BX, CX, DX, AH); 
            break;
        default: 
            printString("Invalid interrupt\n"); 
    } 
}


// Mode yang tersedia dan dapat digunakan
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


// Bonus - ASCII ART
void asciiART() {
    printString("\n");
    printString("  :'#######:::'######::'##:::'##::::'###::::'########:::'#######::'##::: ##:\n");
    printString("  '##.... ##:'##... ##: ##::'##::::'## ##::: ##.... ##:'##.... ##: ###:: ##:\n");
    printString("   ##:::: ##: ##:::..:: ##:'##::::'##:. ##:: ##:::: ##: ##:::: ##: ####: ##:\n");
    printString("   ##:::: ##:. ######:: #####::::'##:::. ##: ##:::: ##: ##:::: ##: ## ## ##:\n");
    printString("   ##:::: ##::..... ##: ##. ##::: #########: ##:::: ##: ##:::: ##: ##. ####:\n");
    printString("   ##:::: ##:'##::: ##: ##:. ##:: ##.... ##: ##:::: ##: ##:::: ##: ##:. ###:\n");
    printString("   ##:::: ##:'##::: ##: ##:. ##:: ##.... ##: ##:::: ##: ##:::: ##: ##:. ###:\n");
    printString("  . #######::. ######:: ##::. ##: ##:::: ##: ########::. #######:: ##::. ##:\n");
    printString("  :.......::::......:::..::::..::..:::::..::........::::.......:::..::::..::\n\n");
    printString("                                 Created By:\n");
    printString("                             - Ronggur (13519008)\n");
    printString("                             - Azhar   (13519020)\n");
    printString("                             - Alge    (13519039)\n");
    printString("\n");
    printString("\n");
    
}


// Bonus - BiosLogo
void biosLogo() {
    // Graphic mode
    int max_X = 320;
    int max_Y = 200;

    char image[512];
    int sec = 1024;
    int address;
    int x_size;
    int y_size;
    int halfDif_x;
    int halfDif_y;
    int x,y,i,nSec;

    nSec = 0;
    readSector(image,sec + nSec);
    
    x_size = image[0];
    y_size = image[1];
    
    // Coba bikin ketengah
    halfDif_x = div((max_X - x_size),2);
    halfDif_y = div((max_Y - y_size),2);

    x = halfDif_x;
    y = halfDif_y;
    
    i = 2;

    while (y < y_size + halfDif_y) {
        while (x < x_size + halfDif_x) {
            if (i >= 512) {
                nSec++;
                readSector(image,sec + nSec);
                i = 0; 
                // printString("a");
            }
            address = 320*y + x;
            putInMemory(0xA000,address,image[i]);
            i++;
            x++;
        }
        x = halfDif_x;
        y++;
    }
}
