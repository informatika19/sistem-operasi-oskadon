#include "lib/fileIO.h"
#include "lib/text.h"
#include "lib/sector.h"

int main() {
    int dump;
    char parentIdx[512];
    char param[512];

    int i,j;
    char sourceFileName[180];
    char targetFileName[180];
    char currFlName[128];
    char maps[512];
    char files[1024];
    char* pathSource;
    char* pathTarget;
    int temp;
    char currParentIdx;
    char tempParentIdx;
    char parentIndex;
    int sectNum, sectPos, sectIdx, fileSourceIdx;
    int found;
    int* foundIdx;
    int dirNum,dirIdx;
    int sectorSourcefileIdx;
    int soft = 0;
    int z = 0;
    int filesNum,filesIdx;

    interrupt(0x21, 0, "Executing ln...! \n\0",0,0);   // Entahlah ini harus ada

    clear(parentIdx,512);
    clear(param,512);

    readSector(parentIdx,800);
    readSector(param,801);

    // currParentIdx = parentIdx[0];
    
    sourceFileName[0] = '\0';
    targetFileName[0] = '\0';

    currParentIdx = parentIdx[0];

    // Copy parameter
    i = 0;
    j = 0;
    // Source File
    i = ignoreSpace(param, i);
    while (param[i] != ' ' && param[i] != '\0')
    {
        sourceFileName[j] = param[i];
        i++;
        j++;
    }

    // Target Folder
    i = ignoreSpace(param, i);
    j = 0;
    while (param[i] != ' ' && param[i] != '\0')
    {
        targetFileName[j] = param[i];
        i++;
        j++;
    }
    
    if(strlen(targetFileName) == 0 || strlen(sourceFileName) == 0){
        printString("ERROR : fileName not valid\n");
        interrupt(0x21, 0xFF06, "bin/shell", 0x2000, &dump);
    }

    pathSource = sourceFileName; // pathSource = ./folder/file ; currDirIdx = 0xFF
    pathTarget = targetFileName;
    //below is from readFile from kernel.c
    
    // 0. inisialisasi
    //readSector(maps, 0x100);
    readSector(files, 0x101);
    readSector(files + 512, 0x102);

    // Copy alamat parentIdx sekarang
    tempParentIdx = currParentIdx;
    // Cari file yang mau dicopy
    // 1. Cari folder dan file
    i = 0;
    j = 0;
    if (pathSource[i] == '/') {   // dari root
        currParentIdx = 0xFF;
        i++;  
    }

    // 2. Cari pathSource sampe ketemu bagian file
    // printString(pathSource);
    // printString("\n");
    clear(currFlName,128);
    while (pathSource[i] != '\0') {
        currFlName[j] = pathSource[i];

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
    currFlName[j] = pathSource[i];

    // Cek apakah nama file valid (tidak boleh kosong)
    if (strcmp(currFlName,"")) {
        printString("Nama file tidak valid\n");
        interrupt(0x21, 0xFF06, "bin/shell", 0x2000, &dump);
    }

    // 3. Cek file ada
    found = isFlExist(files,currParentIdx,currFlName,false,foundIdx);
    if (!found) {
        // Cek folder ada
        printString("ERROR : File tidak ditemukan\n");
        // currParentIdx = temp;
        interrupt(0x21, 0xFF06, "bin/shell", 0x2000, &dump); 
    }

    //printString("File ditemukan\n");
    parentIndex = tempParentIdx; // Kembali ke parentIdx sekarang lagi
    fileSourceIdx = *foundIdx;      // Files idx ditemukan



    //yang dibawah sekarang copy dari write file
    // 2. Cek parrent folder valid
    found = false;
    if (parentIndex == 0xFF) { // folder di root
        found = true;
    } else if (files[16*parentIndex+1] == 0xFF) { // sebuah folder
        found = true;
    }

    if (!found) {
        printString("ERROR : Folder tidak valid\n");
        //*sectors = -4;
        interrupt(0x21, 0xFF06, "bin/shell", 0x2000, &dump);
    }
    

    // 3. Atur bagian path
    // 3a. Buat folder
    i = 0;
    j = 0;
    if (pathTarget[i] == '/') {   // dari root
        currParentIdx = 0xFF;
        i++;  
    } else {
        currParentIdx = parentIndex;
    }

    
    while (pathTarget[i] != '\0') {
        currFlName[j] = pathTarget[i];
        
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
                 //*sectors = -5;
                interrupt(0x21, 0xFF06, "bin/shell", 0x2000, &dump);
            } else {
                // Cek apakah sudah tersedia folder yang sama
                temp = currParentIdx;
                found = isFlExist(files,currParentIdx,currFlName,true,foundIdx);
                if (found) {
                    //printString("Folder sudah ada\n");
                    currParentIdx = *foundIdx;
                    // printString(currParentIdx);
                } else {
                    // cari files yang kosong
                    filesIdx = foundEmptyDir(files);
                    if (filesIdx == -1) {
                        printString("ERROR : Tidak cukup entri di files\n");
                        //*sectors = -2;
                        currParentIdx = temp;
                        interrupt(0x21, 0xFF06, "bin/shell", 0x2000, &dump);
                    }

                    filesNum = filesIdx*16;
                    // buat folder baru
                    writeDir(files,filesNum,currParentIdx,0xFF,currFlName);

                    currParentIdx = filesIdx;
                }
            }
            

            j = 0;  
        } else {
            j++;
        }
        i++;
        
    }
    currFlName[j] = pathTarget[i];
    // Cek apakah nama file valid (tidak boleh kosong, kalau kosong berarti cuma create folder doang)
    
    
    if (strcmp(currFlName,"")) {
        printString("ERROR : Nama file tidak valid\n");
        //*sectors = -5;
        interrupt(0x21, 0xFF06, "bin/shell", 0x2000, &dump);
    }

    // 3b. Cek apakah sudah file udah ada
    found = isFlExist(files,currParentIdx,currFlName,false,foundIdx);
    if (found) {
        printString("ERROR : File sudah ada\n");
        //*sectors = -1;
        interrupt(0x21, 0xFF06, "bin/shell", 0x2000, &dump);
    }
    // cari files yang kosong
    filesIdx = foundEmptyDir(files);
    if (filesIdx == -1) {
        printString("ERROR : Tidak cukup entri di files\n");
        //*sectors = -2;
        interrupt(0x21, 0xFF06, "bin/shell", 0x2000, &dump);
    }
    filesNum = filesIdx*16;


    // 5. Buat File
    writeDir(files,filesNum,currParentIdx,files[fileSourceIdx*16+1],currFlName);

    //writeSector(map,0x100);
	writeSector(files,0x101);
    writeSector(files+512,0x102);
    printString("Linking Success!\n");
    //*sectors = secIdx;

    interrupt(0x21, 0xFF06, "bin/shell", 0x2000, &dump);
    

    // interrupt(0x21, 0xFF06, "bin/shell", 0x2000, &dump);
}