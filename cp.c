#include "lib/boolean.h"
#include "lib/fileIO.h"
#include "lib/sector.h"

void createFolder(char *path, int *result, char parentIndex);

int main() {
    // KAMUS
    int dump;
    int result;
    char parentIdx[512];
    char param[512];

    char files[1024];
    char currFlName[128];
    char sourcePath[512];
    char targetPath[512];
    char sourceBuffer[512*16];
    int sourceIdx;
    int targetIdx;
    int sourceParentIdx;
    char currParentIdx;
    char tempParentIdx;
    bool isFolder;

    int filesIdx = 0;               // idx files
    int filesNum = 16*filesIdx;       // alamat files
    int temp;
    int* foundIdx;
    bool found;
    int i,j;

    interrupt(0x21, 0, "Executing cp...\n\0",0,0);   // JIMAT, GA ADA INI ERROR
    // Bersihkan buffer
    clear(parentIdx,512);
    clear(param,512);
    clear(sourcePath,512);
    clear(targetPath,512);
    
    
    // Baca Parameter dan ParentIdx
    readSector(parentIdx,800);
    readSector(param,801);

    // printString("Test param\n");
    // printString(param);
    // printString("\n");

    // Copy parentIdx
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

    // Target Folder
    i = ignoreSpace(param, i);
    j = 0;
    while (param[i] != ' ' && param[i] != '\0')
    {
        targetPath[j] = param[i];
        i++;
        j++;
    }

    // printString("Test Path\n");
    // printString(sourcePath);
    // printString("\n");
    // printString(targetPath);
    // printString("\n");

    // Test apakah argumen valid;
    if (strlen(sourcePath) == 0 || strlen(targetPath) == 0) {
        printString("Parameter cp tidak valid\n");
        interrupt(0x21, 0xFF06, "bin/shell", 0x2000, &dump);
        return;
    }
    
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
                return;
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
                    return;
                }
            }
            j = 0;  
        } else {
            j++;
        }
        i++;
        
    }
    currFlName[j] = sourcePath[i];

    // printString("tes\n");
    // printString(currFlName);
    // printString("\n");

    // Cek apakah nama file valid (tidak boleh kosong)
    if (strcmp(currFlName,"")) {
        printString("Nama file tidak valid\n");
        interrupt(0x21, 0xFF06, "bin/shell", 0x2000, &dump);
        return;
    }

    // 3. Cek file ada
    found = isFlExist(files,currParentIdx,currFlName,false,foundIdx);
    isFolder = false;
    if (!found) {
        // Cek folder ada
        found = isFlExist(files,currParentIdx,currFlName,true,foundIdx);
        isFolder = true;
        if (!found) {
            printString("File tidak ditemukan\n");
            interrupt(0x21, 0xFF06, "bin/shell", 0x2000, &dump);
            return;
        } 
    }

    //printString("File ditemukan\n");
    sourceParentIdx = currParentIdx;
    currParentIdx = tempParentIdx; // Kembali ke parentIdx sekarang lagi
    sourceIdx = *foundIdx;      // Files idx ditemukan

    targetIdx = currParentIdx;
    // printString("Test target idx\n");
    // printString(currParentIdx);
    // printString("\n");
    // printString("tes\n");

    // copy file
    if (!isFolder) {
        readFile(sourceBuffer, currFlName, &result, sourceParentIdx);
        writeFile(sourceBuffer, targetPath, &result, targetIdx);
    }
    else {
        // copy folder
        i = 0;
        printString("test");
        printString("\n");
        while (targetPath[i] != '\0') {
            i += 1;
        }

        targetPath[i] = '/';
        i += 1;
        for (j = 0; j < strlen(currFlName); j++) {
            targetPath[i] = currFlName[j];
            i += 1;
        }
        printString(targetPath);
        printString("\n");
        createFolder(targetPath, &result, targetIdx);
    }
    
    // printString("Test Pathing Target\n");
    // printString(currFlName);
    // printString("\n");
    
    // Ubah parent folder
    //files[sourceIdx * 16] = targetIdx;
    
    // Tulis kembali
    //writeSector(files, 0x101);
    //writeSector(files + 512, 0x102);

    printString("cp success...\n");
    interrupt(0x21, 0xFF06, "bin/shell", 0x2000, &dump);
    return;
}

void createFolder(char *path, int *result, char parentIndex){
    // path jadi nama
    // KAMUS
    
    char map[512];
    char files[1024];
    int sec = 0x103;
    bool found = false;
    int filesIdx = 0;               // idx files
    int filesNum = 16*filesIdx;       // alamat files
    int secIdx = 0;               // idx sector
    int secNum = 16*secIdx;       // idx map ke sector (alamat files)
    int countSec = 0;
    int i, j, k, l;
    char buffSec[512];
    char buffName[15];
    char currFlName[128];
    char currParentIdx,temp;
    int *foundIdx;
    
    // printString("t1\n");
    // 1. Baca Sektor Map dan Dir
    readSector(map,0x100);
    readSector(files,0x101);
    readSector(files+512,0x102);
     
    //printString("t2\n");
    //2. Cek parrent folder valid
    found = false;
    if (parentIndex == 0xFF) { // folder di root
        found = true;
    } else if (files[16*parentIndex+1] == 0xFF) { // sebuah folder
        found = true;
    }
    //printString(&parentIndex);
    if (!found) {
        printString("Folder tidak valid\n");
        //*sectors = -4;
        return;
    }
    

    // 3. Atur bagian path
    // 3a. Buat folder
    i = 0;
    j = 0;
    if (path[i] == '/') {   // dari root
        currParentIdx = 0xFF;
        i++;  
    } else {
        currParentIdx = parentIndex;
    }

    
    while (i <= strlen(path)) {
        currFlName[j] = path[i];
        
        if (currFlName[j] == '/' || currFlName[j] == '\0') {
            
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
                return;
            } else {
                // Cek apakah sudah tersedia folder yang sama
                temp = currParentIdx;
                found = isFlExist(files,currParentIdx,currFlName,true,foundIdx);
                currParentIdx = temp;
                if (found) {
                    //printString("Folder sudah ada\n");
                    currParentIdx = *foundIdx;
                    // printString(currParentIdx);
                } else {
                    // cari files yang kosong
                    filesIdx = foundEmptyDir(files);
                    if (filesIdx == -1) {
                        printString("Tidak cukup entri di files\n");
                        //*sectors = -2;
                        return;
                    }

                    filesNum = filesIdx*16;
                    // buat folder baru
                    //printString("currFlName");
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
    currFlName[j] = path[i];
    // Cek apakah nama file valid (tidak boleh kosong, kalau kosong berarti cuma create folder doang)
//    writeSector(map,0x100);
    writeSector(files,0x101);
    writeSector(files+512,0x102);
}