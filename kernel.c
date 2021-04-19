#include "module/fileIO.h"
#include "module/folderIO.h"
#include "module/math.h"
#include "module/text.h"

// Fungsi Bawaan
void putInMemory (int segment, int address, char character);
void handleInterrupt21 (int AX, int BX, int CX, int DX);
int interrupt(int number, int AX, int BX, int CX, int DX);
void executeProgram(char *filename, int segment, int *success, char parentIndex);

// Fungsi Graphic dan Hiasan
void opening();
void biosLogo();

int main () {
    int suc,suc2;
    
    // Print OS Logo
    interrupt(0x10,0x0013,0,0,0);   // Mode Graphic
    biosLogo();
    interrupt(0x15,0x8600,0,30,0);  // wait CK:100 = 1000 ms
    interrupt(0x10,0x0003,0,0,0);   // Mode text

    opening();

    makeInterrupt21();
    // writeFile("HEHEH\0","ikkeh.txt\0",suc2,0xFF);
    handleInterrupt21(0XFF06, "shell", 0x2000, &suc);

}


void handleInterrupt21 (int AX, int BX, int CX, int DX){
    char AL, AH; 
    AL = (char) (AX); 
    AH = (char) (AX >> 8); 
    switch (AL) {
        case 0x0:
            printString(BX);
            break;
        case 0x1:
            readString(BX);
            break;
        case 0x2:
            readSector(BX, CX);
            break;
        case 0x3:
            writeSector(BX, CX);
            break;
        case 0x4: 
            readFile(BX, CX, DX, AH); 
            break; 
        case 0x5: 
            writeFile(BX, CX, DX, AH); 
            break;
        case 0x6:
            executeProgram(BX, CX, DX, AH);
            break;
        default:
            printString("Invalid interrupt");
    }
}

void executeProgram(char *filename, int segment, int *success) {
    char buffer[512 * 16];
    int i;
    
    clear(buffer, 512 * 16);
    readFile(&buffer, filename, success, 0xFF);
    if (*success) {
        for (i=0; i<512 * 16; i++) {
            putInMemory(segment, i, buffer[i]);
        }
        launchProgram(segment);
    } else if (*success = -1){
        printString("File tidak ditemukkan");
        return;
    }
}


void opening() {
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


