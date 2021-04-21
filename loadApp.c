//loadedFilee.c
//src: Michael Black, 2007
// Punya orang
//
//Loads a file into the file system
//This should be compiled with gcc and run outside of the OS

#include <stdio.h>
#include <string.h>
#include "module/boolean.h"

/** Fungsi penunjang */
bool isFlExist(char* dir, char parrentIdx, char* name, bool folder, int* foundIdx);
void getFlName(char* files,char filesIdx, char* name);
void writeDir(char* dir, int dirNum, int parrentIdx, int sectorIdx, char* name);
int strcompare(char* a, char* b);


int findFreeEntry(char* entries) {
    int i;
    for (i = 0; i<64; i++) {
        if (entries[i*16] == 0x00 && entries[i*16+1] == 0x00 && entries[i*16+2] == 0x00) {
            return i;
        }
    }
    return -1;
}

int findFreeSector(char* map) {
    int i;
    for (i=0; i<32; i++){
        if (map[i*16] == 0x00 && map[i*16+1] == 0x00 && map[i*16+2] == 0x00){
            return i;
        }
    }
    return -1;
}

int main(int argc, char* argv[])
{
	int i;
	if (argc<2) {
		printf("Nama file tidak boleh kosong\n");
		return -1;
	}

	// Buka sourceFile
	FILE* loadedFile;
	loadedFile = fopen(argv[1],"r");
	if (loadedFile == 0) {
		printf("File tidak ditemukan\n");
		return -1;
	}

	// Buka systemImg
	FILE* system;
	system=fopen("system.img","r+");
	if (system==0) {
		printf("system.img tidak ditemukan\n");
		return -1;
	}

	// load map
    // map di 0x100
	char map[512];
	fseek(system,512*0x100,SEEK_SET);
	for(i=0; i<512; i++) {
		map[i]=fgetc(system);
	}

	// load files
    // files sector 0x101-0x102
	char files[1024];
	fseek(system,512*0x101,SEEK_SET);
	for (i=0; i<1024; i++) {
		files[i]=fgetc(system);
	}


	// Buat folder bin jika belum ada
	int binFolderIdx;	// idx bin
	bool binExist = isFlExist(files,0xFF,"bin",true,&binFolderIdx);
	if (!binExist) {
		//Cari index kosong di files
		binFolderIdx = findFreeEntry(files);	// Idx files
		if (binFolderIdx == -1) {
			printf("Tidak cukup tempat di files directory\n");
			return -1;
		}

		int filesNum = binFolderIdx*16;
		// buat folder baru
		writeDir(files,filesNum,0xFF,0xFF,"bin");
	}
	


	//Cari index kosong di files
	int dirIdx = findFreeEntry(files);	// Idx files
	if (dirIdx == -1) {
		printf("Tidak cukup tempat di files directory\n");
		return -1;
	}
		
    //Cari sector yang masih kosong di map
    int sectorIdx = findFreeSector(map);
    if (sectorIdx == -1){
        printf("Tidak cukup sektor yang tersedia\n");
        return -1;
    }

	// Hitung panjang file
    fseek(loadedFile, 0, SEEK_END);
    long int fileSize = ftell(loadedFile);
    rewind(loadedFile);

	// Hitung sector yang dibutuhkan 
    int sectorLength;
    if (fileSize % 512 == 0){
        sectorLength = fileSize/512;
    }else{
        sectorLength = fileSize/512 + 1;
    }

    // Cek apakah sector melebihi batas
    if (sectorLength > 16){
        printf("Files terlalu besar!\n");
        return -1;
    }
	
	
	//Bersihkan nama file
	for (i=0; i<16; i++) {
		files[dirIdx*16 + i] = '\0';
	}
		

	//Copy nama file
	for (i=0; i<14; i++)
	{
		if(argv[1][i] == '\0') {
			break;
		}
		files[dirIdx*16 + i + 2] = argv[1][i];
	}
	
    //Isi parent Idx, default root
    // files[dirIdx*16] = 0xFF;
	files[dirIdx*16] = binFolderIdx;   

    //isi sectorIdx
    files[dirIdx*16+1] = sectorIdx;
	
    
	//Masukkan file ke dalam system
	
	int idx=0;
	while(!feof(loadedFile))
	{
		//Tandai map sudah digunakan
		map[sectorIdx*16 + idx]=0xFF;

		// Masukkan ke dalam filesystem
		fseek(system,(0x103 + sectorIdx*16 + idx)*512,SEEK_SET);
		for (i=0; i<512; i++) {
			if (feof(loadedFile)) {
				fputc(0x0, system);
				break;
			}
			else {
				char c = fgetc(loadedFile);
				fputc(c, system);
			}	
		}
		idx++;
	}
	// printf("test\n");

	// Tulis kembali ke system
	fseek(system,512*0x100,SEEK_SET);
	for(i=0; i<512; i++)
	fputc(map[i],system);
	
	fseek(system,512*0x101,SEEK_SET);
	for (i=0; i<1024; i++)
	fputc(files[i],system);

	// fseek(system,512*0x103,SEEK_SET);
	// for (i=0; i<512; i++)
	// fputc(sectors[i],system);

	fclose(system);
	fclose(loadedFile);
	printf("%s diload ke sector idx %d\n", argv[1], sectorIdx);
    printf("loadApp Success!\n");
	return 0;
}



// Fungsionalitas Tambahan
bool isFlExist(char* dir, char parrentIdx, char* name, bool folder, int* foundIdx) {
    // return index file yang ditemukan pada foundIdx
    int filesIdx,j;
    char buffName[15];
    bool found = false;
	
    filesIdx = 0;
    while (filesIdx < 64 && !found) {
        if (dir[16*filesIdx] == parrentIdx) {            // kalau ternyata ada yang parent indexnya sama
			getFlName(dir,filesIdx,buffName);
            if (strcompare(buffName,name)) {         // cek apakah namanya sama
                if ((folder && dir[16*filesIdx+1] == ((char)0xFF)) || (!folder && dir[16*filesIdx+1] != ((char) 0xFF))) {  // cek jenisnya sama
                    found = true;
                }   
            }
        }
        filesIdx++; 
    }
    *foundIdx = --filesIdx;
    return found;
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

int strcompare(char* a, char* b) {
    int i;

    if (strlen(a) != strlen(b)) {
		
        return 0;
    }

    i = 0;
    while (i < strlen(a)) {
        if(a[i] != b[i]) {
			
            return 0;
        }
        i++;
    }

    return 1;
}
