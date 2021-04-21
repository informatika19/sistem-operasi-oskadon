#include "module/boolean.h"
#include "module/fileIO.h"
#include "module/sector.h"

int main() {
    // KAMUS
    int dump;
    char parentIdx[512];
    char param[512];

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

    interrupt(0x21, 0, "Executing mv...\n\0",0,0);   // Entahlah ini harus ada
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
        printString("Parameter tidak valid\n");
        interrupt(0x21, 0xFF06, "shell", 0x2000, &dump);
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
                interrupt(0x21, 0xFF06, "shell", 0x2000, &dump);
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
                    interrupt(0x21, 0xFF06, "shell", 0x2000, &dump);
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
        interrupt(0x21, 0xFF06, "shell", 0x2000, &dump);
    }

    // 3. Cek file ada
    found = isFlExist(files,currParentIdx,currFlName,false,foundIdx);
    if (!found) {
        // Cek folder ada
        found = isFlExist(files,currParentIdx,currFlName,true,foundIdx);
        if (!found) {
            printString("File tidak ditemukan\n");
            interrupt(0x21, 0xFF06, "shell", 0x2000, &dump);
        } 
    }

    //printString("File ditemukan\n");
    currParentIdx = tempParentIdx; // Kembali ke parentIdx sekarang lagi
    sourceIdx = *foundIdx;      // Files idx ditemukan

    // printString("Test Pathing Source\n");
    // printString(currFlName);
    // printString("\n");


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
    clear(currFlName,128);
    
    while (i <= strlen(targetPath)) {
        currFlName[j] = targetPath[i];
        
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
                // printString("Error: Nama folder tidak valid\n");
                // interrupt(0x21, 0xFF06, "shell", 0x2000, &dump);
            } else {
                // Cek apakah sudah tersedia folder yang sama
                temp = currParentIdx;
                found = isFlExist(files,currParentIdx,currFlName,true,foundIdx);
                currParentIdx = temp;
                if (found) {
                    currParentIdx = *foundIdx;
                    // printString(currParentIdx);
                } else {
                    // cari files yang kosong
                    filesIdx = foundEmptyDir(files);
                    if (filesIdx == -1) {
                        printString("Tidak cukup entri di files\n");
                        interrupt(0x21, 0xFF06, "shell", 0x2000, &dump);
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
    // printString("test keluar\n");
    // printString(currFlName);
    // printString("\n");

    targetIdx = currParentIdx;
    
    // printString("Test Pathing Target\n");
    // printString(currFlName);
    // printString("\n");
    
    // Ubah parent folder
    files[sourceIdx * 16] = targetIdx;
    
    // Tulis kembali
    writeSector(files, 0x101);
    writeSector(files + 512, 0x102);

    printString("mv success...\n");
    interrupt(0x21, 0xFF06, "shell", 0x2000, &dump);

}