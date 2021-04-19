#include "fileIO.h"

void readFile(char *buffer, char *path, int *result, char parentIndex){
    char map[512];
    char files[512*2];
    char sectors2[512];
    int fileIdx, found, pathLength;
    // FLOWCHART :: Begin
    // FLOWCHART :: Baca Sektor DIR
    readSector(files, 0x101); 
    readSector(files+512, 0x102);
    readSector(sectors2, 0x103);
    
    
    // FLOWCHART::Mencari file yang namanya sesuai
    pathLength = stringLength(path, 14);
    fileIdx = -1;
    found = 0;
    while(!found && fileIdx<32){
        fileIdx++;
        found = stringLength(files+(16*fileIdx+2),14) == pathLength;
        found = found && stringEqual(path, files+(16*fileIdx+2), pathLength);
        found = found && (files[16*fileIdx] == parentIndex);
    }
    if (!found) {
            *result = -1;
            // printString("ga nemu gaes\r\n");
        }
        else {
            int secIdx, secPos, i;
        secIdx = files[fileIdx * 16 + 1];

        for (i = 0; i < 16; i++) {
        secPos = secIdx * 16 + i;
        if (sectors2[secPos] == 0) {
            //printString("End of file..\n\r");
            break;
        }
        //printString("Reading sector\n\r");
        // clear(buffer + (i * 512), 512);
        readSector(buffer + (i * 512), sectors2[secPos]);
        }
        
        *result = 1;
        //ini kayanya fix
            // printString("ini bisa gaes\r\n");
    }
}

void writeFile(char *buffer, char *path, int *sectors, char parentIndex){
    // map berukuran 512 bytes, files berukuran 2*512 bytes, sectors berukuran 512 bytes
    int NOT_ENOUGH_FILE = -2;
    int NOT_ENOUGH_SECTOR = -3;

    char map[512];
    char files[512*2];
    char sectors2[512];
    int i, j, emptyFilesIndex, sectorCount, sectorsCount, sameFilename, sectorNeed;
    char currdir = parentIndex;
    sectorNeed = div(stringLengthnoMax(buffer)+511,512);

    // FLOWCHART::Begin
    // FLOWCHART::Baca Sektor MAP dan DIR
    readSector(map, 256); // map di 0x100 
    //files di 0x101 dan 0x102 
    readSector(files, 0x101); 
    readSector(files+512, 0x102);
    readSector(sectors2, 0x103);
    
    
    // FLOWCHART::Cek Dir yang Kosong
    // Mencari entry file pada sektor files yang masih kosong
    // 64 adalah jumlah maksimal file, satu entry pada filesystem berukuran 16 bytes
    emptyFilesIndex=0;
    for (emptyFilesIndex; emptyFilesIndex < 64; emptyFilesIndex++) {
        // Jika ada entry yang kosong
            if (files[(emptyFilesIndex*16)+2] == '\0') {
        // printString("files kosong \r\n\0");
                break;
            }
        }
    // FLOWCHART::Tidak Ada -> Hentikan Proses Penulisan File
    // Apabila jumlah entry file tidak cukup (kosong di index terakhir)
    if (emptyFilesIndex == 64) {
            *sectors = NOT_ENOUGH_FILE;
        // printString("files penuh \r\n\0");
        return;
        }
    
    // FLOWCHART::Cek Jumlah Sector di Map + sectors(?), cukup untuk buffer file
    // Membaca ketersediaan sektor di map
    // Byte pada sektor map menandakan apakah sektor terisi atau belum terisi pada filesystem
    sectorCount = 0;
    for (i = 0 ; i < 256 && sectorCount < sectorNeed; i++){
        //hitung jumlah sektor di map yang bisa diisi
        if (map[i] == 0x00){  
        sectorCount++;
        // printString("nol found \r\n\0");
        }
    }
    
    sectorsCount = 0;
    // Mengecek apakah ada baris di sectors yang masih bisa diisi, jumlah baris di sectors ada 32
    for(sectorsCount = 0; sectorsCount < 32; sectorsCount++){
    // Jika ada baris yang kosong
        if((sectors2[sectorsCount*16]) == 0x00){ 
    // printString("sectors kosong found \r\n\0");
            break;
        }
    }
    // FLOWCHART::Tidak cukup -> Hentikan Proses Penulisan File
    // Apabila jumlah sektor tidak cukup
    if (sectorCount < sectorNeed) {
        *sectors = NOT_ENOUGH_SECTOR; 
    // printString("Sektor map tidak cukup \r\n\0");
    return;
    }
    // Apabila tidak ada baris di sectors yang bisa diisi
    if(sectorsCount == 32) {
        *sectors = NOT_ENOUGH_SECTOR;
    // printString("entry sectors tidak cukup \r\n\0");
        return;
    }
    // Validasi folder
    // Blom kebayang caranya 
    
    // Validasi apakah ada file dengan nama sama
    for (i = 0; i< 64; i++) {
        if ((files[i * 16] == parentIndex)){
        sameFilename=1;
        j= 0;
        while(j<14 && sameFilename==1){
            if ((path[j]=="\0" && files[i*16+2+j] !="\0") ||(path[j]!="\0" && files[i*16+2+j] =="\0") ){
            sameFilename=0;
            break;
            }
                if(path[j]!=files[i*16+2+j]){
                sameFilename=0;
            }
            j++;
        }
            if (sameFilename==1){
                // Ada file dengan nama yang sama di directory parent
                // printString("file sudah ada \r\n\0");
                *sectors = -1;
                    return;
            }
        } 
    }

    // FLOWCHART::Bersihkan sektor yang akan digunakan untuk menyimpan nama
    // Bersihkan isi dari baris pada array files yang mau digunakan
    clear(files + (emptyFilesIndex*16), 16);
    // Isi indeks entri pada tabel sektor dan isi parent dari file
    files[emptyFilesIndex*16+ 1] = sectorsCount;
    files[emptyFilesIndex*16] = parentIndex;

    // FLOWCHART::Isi sektor pada dir dengan nama file
    // Nama file adalah 14 bytes terbelakang dari satu entry file
    for (i = 0; i < 14; i++) {
            if (path[i] == '\0')
                break;
            files[emptyFilesIndex*16 + 2 + i] = path[i];
        }

    // FLOWCHART::Cari sektor di map yang kosong -> isi sektor tersebut dengan byte di buffer file dan tandai di map
    for (i = 0; i < sectorNeed; i++) {
        for (j = 0 ; j < 256; j++){
        // Cari sektor di map yang kosong
            if (map[j] == 0x00){  
                // Tandai di Map
                map[j] = 0xFF;
                sectors2[(sectorsCount*16) + i] = j;
                // Isi sektor dengan byte buffer file
                writeSector(buffer + (i*512), j);
            }
        }
    }
    
    // rewrite the sector
    writeSector(map, 0x100);
    writeSector(files, 0x101);
    writeSector(files+512, 0x102);
    // writeSector(sectors, 0x103);
    writeSector(sectors2, 0x103);

    *sectors = 1;
}

void deleteFile(char *buffer, char *path, int *result, char parentIndex) {
    char map[512];
    char files[512*2];
    char sectors2[512];
    int fileIdx, found, pathLength, mapIdx;
    // FLOWCHART :: Begin
    // FLOWCHART :: Baca Sektor DIR
    readSector(map, 0x100);
    readSector(files, 0x101); 
    readSector(files+512, 0x102);
    readSector(sectors2, 0x103);
    
    // FLOWCHART::Mencari file yang namanya sesuai
    pathLength = stringLength(path, 14);
    fileIdx = -1;
    found = 0;
    while(!found && fileIdx<32){
        fileIdx++;
        found = stringLength(files+(16*fileIdx+2),14) == pathLength;
        found = found && stringEqual(path, files+(16*fileIdx+2), pathLength);
        found = found && (files[16*fileIdx] == parentIndex);
    }
    if (!found) {
        *result = -1;
        // printString("ga nemu gaes\r\n");
    } else {
        int secIdx, secPos, i;
        // Dapatkan sectorIdx
        secIdx = files[fileIdx * 16 + 1];

        for (i = 0; i < 16; i++) {
            // sector posisi
            secPos = secIdx * 16 + i;
            if (sectors2[secPos] == 0) {
                break;
            }
            //delete map
            mapIdx = sectors2[secPos];
            map[mapIdx] = 0x00;

            // delete sectors2
            sectors2[secPos] = 0x00;
        }

        // delete entry files
        for (i = 0; i < 16; i++) {
            files[fileIdx * 16 + i] = 0x00;
        }
        
        
        *result = 1;
        //ini kayanya fix
        // printString("ini bisa gaes\r\n");
    }
}


void readSector(char *buffer, int sector){
  interrupt(0x13, 0x201, buffer, div(sector,36)*0x100 + mod(sector,18)+1, mod(div(sector,18),2)*0x100);
}
void writeSector(char *buffer, int sector){
  interrupt(0x13, 0x301, buffer, div(sector,36)*0x100 + mod(sector,18)+1, mod(div(sector,18),2)*0x100);
}