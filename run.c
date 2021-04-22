#include "module/fileIO.h"
#include "module/text.h"
#include "module/sector.h"

int main() {
    int dump;
    char parentIdx[512];
    char param[512];
    char currParentIdx;
    char toBeParam[512];

    char fileName[512];
    int fileParent;
    
    char files[1024];
    char currFlName[128];
    char sourcePath[512];
    int sourceIdx;
    char tempParentIdx;
    int filesIdx = 0;               // idx files
    int filesNum = 16*filesIdx;       // alamat files
    int temp;
    int* foundIdx;
    bool found;
    int i,j;

    interrupt(0x21, 0, "Executing app..! \n\0",0,0);   // Entahlah ini harus ada

    clear(parentIdx,512);
    clear(param,512);

    readSector(parentIdx,800);
    readSector(param,801);

    currParentIdx = parentIdx[0];

    // Copy parameter
    i = 0;
    j = 0;
    // Source File
    i = ignoreSpace(param, i);
    while (param[i] != ' ' && param[i] != '\0')
    {
        sourcePath[j] = param[i];
        i++;
        j++;
    }

    // toBe Parameter
    clear(toBeParam,512);
    i = ignoreSpace(param, i);
    strcpy(toBeParam, param + i);

    
    // 0. inisialisasi
    readSector(files, 0x101);
    readSector(files + 512, 0x102);

    // Copy alamat parentIdx sekarang
    tempParentIdx = currParentIdx;
    // Cari file yang mau dicopy
    // 1. Cari folder dan file
    i = 0;
    j = 0;
    if (sourcePath[i] == '/') {   // dari root
        currParentIdx = 0xFF;
        i++;  
    }

    // 2. Cari sourcePath sampe ketemu bagian file
    // printString(sourcePath);
    // printString("\n");
    clear(currFlName,128);
    while (sourcePath[i] != '\0') {
        currFlName[j] = sourcePath[i];

        if (currFlName[j] == '/') {
            currFlName[j] = '\0';
            if (strcmp(currFlName,".")) 
            {
                // Do Nothing, di curr dir yang sama
            } 
            else if (strcmp(currFlName,"..")) 
            {   // Back to parent
                if (currParentIdx != 0xFF) {    // Bukan di root
                    // Cari parent folder ini
                    currParentIdx = files[(currParentIdx)*16];
                }
                // kalau di root do nothing aja
            } 
            else if (strcmp(currFlName,"")) 
            {
                printString("Error: Nama folder tidak valid\n");
                interrupt(0x21, 0xFF06, "bin/shell", 0x2000, &dump);
            } else {
                // Cek apakah sudah tersedia folder yang sama
                temp = currParentIdx;
                found = isFlExist(files,currParentIdx,currFlName,true,foundIdx);
                currParentIdx = temp;
                if (found) {
                    // printString("Folder sudah ada\n");
                    currParentIdx = *foundIdx;
                    // printString(currParentIdx);
                } else {
                    printString("Error: Folder tidak ditemukan\n");
                    interrupt(0x21, 0xFF06, "bin/shell", 0x2000, &dump);
                }
            }
            j = 0;  
        } else {
            j++;
        }
        i++;
        
    }
    currFlName[j] = sourcePath[i];

    // Cek apakah nama file valid (tidak boleh kosong)
    if (strcmp(currFlName,"")) {
        printString("Nama file tidak valid\n");
        interrupt(0x21, 0xFF06, "bin/shell", 0x2000, &dump);
    }

    // 3. Cek file ada
    found = isFlExist(files,currParentIdx,currFlName,false,foundIdx);
    if (!found) {
        // Cek folder ada
        printString("File tidak ditemukan\n");
        interrupt(0x21, 0xFF06, "bin/shell", 0x2000, &dump); 
    }

    //printString("File ditemukan\n");
    currParentIdx = tempParentIdx; // Kembali ke parentIdx sekarang lagi
    sourceIdx = *foundIdx;      // Files idx ditemukan

    clear(fileName,512);
    filesNum = sourceIdx*16;
    strcpy(fileName,files + (filesNum + 2));
    fileParent = files[filesNum];
    fileParent = (fileParent << 8) + 0x06;

    writeSector(toBeParam,801);

    printString(fileName);
    printString(" run!\n");
    interrupt(0x21,fileParent, fileName, 0x2000, &dump);


    interrupt(0x21, 0xFF06, "bin/shell", 0x2000, &dump);
    // return 1;
}