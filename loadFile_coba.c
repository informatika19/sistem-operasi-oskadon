//loadedFilee.c
//src: Michael Black, 2007
// Punya orang
//
//Loads a file into the file system
//This should be compiled with gcc and run outside of the OS

#include <stdio.h>

int findFreeEntry(char* entries) {
    int i;
    for (i = 0; i<64; i++) {
        if (entries[i*16] == 0x00) {
            return i;
        }
    }
    return -1;
}

int findFreeSector(char* map) {
    int i;
    for (i=0; i<32; i++){
        if (map[i*16] == 0x00){
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
	for (i=0; i<16; i++)
		files[dirIdx*16+i]=0x00;

	//Copy nama file
	for (i=0; i<14; i++)
	{
		if(argv[1][i]==0) {
			break;
		}
			
		files[dirIdx*16+i+2] = argv[1][i];
	}
	
    //Isi parent Idx, default root
    files[dirIdx*16] = 0xFF;   

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
    printf("loadFile Success!\n");
	return 0;
}
