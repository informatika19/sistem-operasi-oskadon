#include "module/boolean.h"
#include "module/fileIO.h"
#include "module/sector.h"

int main() {
    // KAMUS
    char files[1024];
    char currFlName[128];
    char sourcePath[512];
    char targetPath[512];
    int sourceIdx;
    int targetIdx;
    char currParentIdx;
    char tempParentIdx;

    int filesIdx = 0;               // idx files
    int filesNum = 16*filesIdx;       // alamat files
    int temp;
    int* foundIdx;
    bool found;
    int i,j;

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

    // Cek apakah nama file valid (tidak boleh kosong)
    if (strcmp(currFlName,"")) {
        printString("Nama file tidak valid\n");
        return;
    }

    // 3. Cek file ada
    found = isFlExist(files,currParentIdx,currFlName,false,foundIdx);
    if (!found) {
        printString("File tidak ditemukan\n");
        return;
    }

    //printString("File ditemukan\n");
    currParentIdx = tempParentIdx; // Kembali ke parentIdx sekarang lagi
    sourceIdx = *foundIdx;      // Files idx ditemukan

    // 1. cari folder target
    i = 0;
    j = 0;
    if (targetPath[i] == '/') {   // dari root
        currParentIdx = 0xFF;
        i++;  
    }

    // 2. Cari targetPath sampe ketemu bagian file
    // printString(targetPath);
    // printString("\n");
    while (targetPath[i] != '\0') {
        currFlName[j] = targetPath[i];
        
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
    currFlName[j] = targetPath[i];

    // Cek apakah nama folder valid (tidak boleh kosong)
    if (strcmp(currFlName,"")) {
        printString("Nama folder tidak valid\n");
        return;
    }

    found = isFlExist(files,currParentIdx,currFlName,true,foundIdx);
    if (!found) {
        printString("Folder tujuan tidak ditemukan\n");
        return;
    }

    // Idx folder yang ingin dijadikan parent
    targetIdx = *foundIdx;


    // Ubah parent folder
    files[sourceIdx * 16 + 1] = targetIdx;
    
    // Tulis kembali
    writeSector(files, 0x101);
    writeSector(files + 512, 0x102);

    return;

}