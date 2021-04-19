#include "fileIO.h"
#include "sector.h"
#include "string.h"

void writeFile(char *buffer, char *path, int *sectors, char parentIndex) {
    // path jadi nama file dan folder

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
     
    // printString("t2\n");
    // 2. Cek parrent folder valid
    found = false;
    if (parentIndex == 0xFF) { // folder di root
        found = true;
    } else if (files[16*parentIndex+1] == 0xFF) { // sebuah folder
        found = true;
    }

    if (!found) {
        printString("Folder tidak valid\n");
        *sectors = -4;
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

    
    while (path[i] != '\0') {
        currFlName[j] = path[i];
        
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
                 *sectors = -5;
                return;
            } else {
                // Cek apakah sudah tersedia folder yang sama
                temp = currParentIdx;
                found = isFlExist(files,currParentIdx,currFlName,true,foundIdx);
                currParentIdx = temp;
                if (found) {
                    printString("Folder sudah ada\n");
                    currParentIdx = *foundIdx;
                    // printString(currParentIdx);
                } else {
                    // cari files yang kosong
                    filesIdx = foundEmptyDir(files);
                    if (filesIdx == -1) {
                        printString("Tidak cukup entri di files\n");
                        *sectors = -2;
                        return;
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
    currFlName[j] = path[i];
    // Cek apakah nama file valid (tidak boleh kosong, kalau kosong berarti cuma create folder doang)
    
    
    if (strcmp(currFlName,"")) {
        printString("Nama file tidak valid\n");
        *sectors = -5;
        return;
    }

    // 3b. Cek apakah nama file udah ada
    found = isFlExist(files,currParentIdx,currFlName,false,foundIdx);
    if (found) {
        printString("File sudah ada\n");
        *sectors = -1;
        return;
    }
    // cari files yang kosong
    filesIdx = foundEmptyDir(files);
    if (filesIdx == -1) {
        printString("Tidak cukup entri di files\n");
        *sectors = -2;
        return;
    }
    filesNum = filesIdx*16;


    // 4. Cek jumlah sektor di map cukup untuk buffer file
    // 1 File mengisi 16 sector (asumsi jadiin chunk gini)
    // Terdapat 32 secIdx yang bisa diisi (chunk), dengan 1 chunk berisi 16 sektor
    // 1 sector memuat 512 byte
    found = false;
    while(secIdx < 32 && !found) {
        if (map[secNum] == 0x00) {
            found = true;
        } else {
            secIdx++;
            secNum = 16*secIdx;
        }
    }
    if (!found) {
        printString("Tidak cukup sektor kosong\n");
        sectors = -3;
        return;
    }

    // 5. Buat File
    writeDir(files,filesNum,currParentIdx,secIdx,currFlName);


    // 6. Cari Sektor di Map yang kosong (sudah ketemu)
    // 7. Tulis Semua Buffer
    // secNum = 16+secIdx*16    
    // 16 sector kernel, secIdx*16 karena setiap file ada 16 sector + i
    // Implementasi panjang buffer
    countSec = div(strlen(buffer)-1,512) + 1;
    for (i = 0; i < countSec; i++) {
        map[secNum+i] = 0xFF;
        for (j = 0; j < 512; j++) {
            buffSec[j] = buffer[j+512*i];
        } 
        writeSector(buffSec,sec+secNum+i); 
    }
    

    writeSector(map,0x100);
	writeSector(files,0x101);
    writeSector(files+512,0x102);
    printString("Write File Success!\n");
    *sectors = secIdx;
    return;
}


void readFile(char *buffer, char *path, int *result, char parentIndex) {
    char files[1024], maps[512];
    int sectNum, sectPos, sectIdx, fileIdx;
    char currParentIdx,temp;
    char currFlName[128];
    bool found;
    int* foundIdx;
    int i,j;

    // 0. inisialisasi
    readSector(maps, 0x100);
    readSector(files, 0x101);
    readSector(files + 512, 0x102);

    // 1. cari folder dan file
    i = 0;
    j = 0;
    if (path[i] == '/') {   // dari root
        currParentIdx = 0xFF;
        i++;  
    } else {
        currParentIdx = parentIndex;
    }

    // 2. Cari path sampe ketemu bagian file
    // printString(path);
    // printString("\n");
    while (path[i] != '\0') {
        currFlName[j] = path[i];

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
                 *result = -5;
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
                    printString("Folder tidak ditemukan\n");
                    *result = -1;
                    return;
                }
            }
            j = 0;  
        } else {
            j++;
        }
        i++;
        
    }
    currFlName[j] = path[i];

    // Cek apakah nama file valid (tidak boleh kosong)
    if (strcmp(currFlName,"")) {
        printString("Nama file tidak valid\n");
        *result = -5;
        return;
    }

    // 3. Cek file ada
    found = isFlExist(files,currParentIdx,currFlName,false,foundIdx);
    if (!found) {
        printString("File tidak ditemukan\n");
        *result = -1;
        return;
    }

    //printString("File ditemukan\n");
    fileIdx = *foundIdx;

    // Read sector index
    sectIdx = files[fileIdx * 16 + 1];

    // Read file
    // Ada 16 sektor
    for (sectNum = 0; sectNum < 16; sectNum++) {
        sectPos = sectIdx * 16 + sectNum;
        
        if (maps[sectPos] == 0x00) { // EOF
            break;
        }
        readSector(buffer + (sectNum * 512), 0x103 + sectPos);
    }
    *result = 1;
}


// Delete File
void deleteFile(char *path, int *result, char parentIndex) {
    char files[1024], maps[512];
    int sectNum, sectPos, sectIdx, fileIdx;
    char currParentIdx,temp;
    char currFlName[128];
    bool found;
    int* foundIdx;
    int i,j;
    
    // 0. inisialisasi
    readSector(maps, 0x100);
    readSector(files, 0x101);
    readSector(files + 512, 0x102);

    // 1. cari folder dan file
    i = 0;
    j = 0;
    if (path[i] == '/') {   // dari root
        currParentIdx = 0xFF;
        i++;  
    } else {
        currParentIdx = parentIndex;
    }

    // 2. Cari path sampe ketemu bagian file
    while (path[i] != '\0') {
        currFlName[j] = path[i];

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
                 *result = -5;
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
                    printString("Folder tidak ditemukan\n");
                    *result = -1;
                    return;
                }
            }
            j = 0;  
        } else {
            j++;
        }
        i++;
        
    }
    currFlName[j] = path[i];

    // Cek apakah nama file valid (tidak boleh kosong)
    if (strcmp(currFlName,"")) {
        printString("Nama file tidak valid\n");
        *result = -5;
        return;
    }

    // 3. Cek file ada
    found = isFlExist(files,currParentIdx,currFlName,false,foundIdx);
    if (!found) {
        printString("File tidak ditemukan\n");
        *result = -1;
        return;
    }

    //printString("File ditemukan\n");
    fileIdx = *foundIdx;

    // Read sector index
    sectIdx = files[fileIdx * 16 + 1];

    // Delete sector di maps
    // secIdx*16 karena setiap file ada 16 sector + i
    for (i = 0; i < 16; i++) {
        sectPos = sectIdx * 16 + i;
        maps[sectPos] == 0x00; // EOF      
    }

    // Delete entry di files
    for (i = 0; i < 16; i++) {
        files[fileIdx * 16 + i] = 0x00;
    }
    
    writeSector(maps,0x100);
	writeSector(files,0x101);
    writeSector(files+512,0x102);
    printString("Delete File Success!\n");

}


// Fungsionalitas Tambahan
bool isFlExist(char* dir, int parrentIdx, char* name, bool folder, int* foundIdx) {
    // return index file yang ditemukan pada foundIdx
    int filesIdx,j;
    char buffName[15];
    bool found = false;

    filesIdx = 0;
    while (filesIdx < 64 && !found) {
        if (dir[16*filesIdx] == parrentIdx) {            // kalau ternyata ada yang parent indexnya sama
            getFlName(dir,filesIdx,buffName);
            if (strcmp(buffName,name)) {         // cek apakah namanya sama
                
                if ((folder && dir[16*filesIdx+1] == 0xFF) || (!folder && dir[16*filesIdx+1] != 0xFF)) {  // cek jenisnya sama
                    found = true;
                }   
            }
        }
        filesIdx++; 
    }
    *foundIdx = --filesIdx;
    return found;
}

int foundEmptyDir(char* dir) {
    // return dirIdx jika ketemu yang kosong, return -1 jika tidak ditemukan
    bool found = false;
    int dirIdx = 0;
    int dirNum = 16*dirIdx;
    while (dirIdx < 64 && !found) {     // ada 64 dir yang bisa diisi
        if (dir[dirNum] == 0x00 && dir[dirNum+1] == 0x00 && dir[dirNum+2] == 0x00){    // Kemungkinan ada bug
            found = true;
        } else {
            dirIdx++;
            dirNum = 16*dirIdx;
        }
    }

    if (found) {
        return dirIdx;
    } else {
        return -1;
    }
    
}

void getFlName(char* files,char filesIdx, char* name) {
    int i;

    for (i = 0; i < 14; i++) {
        name[i] = files[16*filesIdx+(2+i)];    
    }
    name[14] = '\0';
}


void writeDir(char* files, int filesNum, int parrentIdx, int sectorIdx, char* name) {
    int i,j;

    files[filesNum] = parrentIdx;
    files[filesNum+1] = sectorIdx;
    i = filesNum + 2;
    
    j = 0;
    while (i < filesNum+15 && j < strlen(name)) { 
        files[i] = name[j];
        i++;
        j++;
    }
    while (i < filesNum+16) {    // Padding yang kosong
        files[i] = 0x0;
        i++;
    }
}