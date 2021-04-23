#include "folderIO.h"
#include "fileIO.h"
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
                    printString("Folder sudah ada\n");
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


void deleteFolder(char *path, int *result, char parentIndex){
    // // path jadi nama
    // // KAMUS
    
    // char map[512];
    // char files[1024];
    // int sec = 0x103;
    // bool found = false;
    // int filesIdx = 0;               // idx files
    // int filesNum = 16*filesIdx;       // alamat files
    // int secIdx = 0;               // idx sector
    // int secNum = 16*secIdx;       // idx map ke sector (alamat files)
    // int countSec = 0;
    // int i, j, k, l;
    // char buffSec[512];
    // char buffName[15];
    // char currFlName[128];
    // char currParentIdx,temp;
    // int *foundIdx;
    
    // // 1. Baca Sektor Map dan Dir
    // readSector(map,0x100);
    // readSector(files,0x101);
    // readSector(files+512,0x102);

    // // 2. Cek parrent folder valid
    // found = false;
    // if (parentIndex == 0xFF) { // folder di root
    //     found = true;
    // } else if (files[16*parentIndex+1] == 0xFF) { // sebuah folder
    //     found = true;
    // }

    // if (!found) {
    //     printString("Folder tidak ditemukan\n");
    //     return;
    // }
    // printString("writeDir(files,filesNum,currParentIdx,0xFF,);\n");
    // filesIdx = foundEmptyDir(files);
    // filesNum = filesIdx*16;
    // writeDir(files,filesNum,currParentIdx,0xFF," ");
    
    // writeSector(map,0x100);
    // writeSector(files,0x101);
    // writeSector(files+512,0x102);
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
    found = isFlExist(files,currParentIdx,currFlName,true,foundIdx);
    if (!found) {
        *result = -1;
        return;
    }

    //printString("File ditemukan\n");
    fileIdx = *foundIdx;

    // Read sector index
    sectIdx = files[fileIdx * 16 + 1];

    // Delete sector di maps
    // secIdx*16 karena setiap file ada 16 sector + i
    // for (i = 0; i < 16; i++) {
    //     sectPos = sectIdx * 16 + i;
    //     maps[sectPos] == 0x00; // EOF      
    // }

    // Delete entry di files
    for (i = 0; i < 16; i++) {
        files[fileIdx * 16 + i] = 0x00;
    }
    
    writeSector(maps,0x100);
	writeSector(files,0x101);
    writeSector(files+512,0x102);
    printString("Delete Folder Success!\n");
}


void deleteFolderFlagedR(char *path, int *result, char parentIndex){
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
    
    // 1. Baca Sektor Map dan Dir
    readSector(map,0x100);
    readSector(files,0x101);
    readSector(files+512,0x102);

    // recursive delete folder

}