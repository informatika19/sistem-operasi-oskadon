// loadFile.c
// Michael Black, 2007

// Loads a file into the file system
// This should be compiled with gcc and run outside of the OS

#include <stdio.h>

void main(int argc, char* argv[]) {
    int i;

    if (argc < 2) {
        printf("Specify file name to load\n");
        return;
    }

    // open the source file
    FILE* loadedfile;
    loadedfile = fopen(argv[1], "r");
    if (loadedfile == 0) {
        printf("File not found\n");
        return;
    }

    // open the system image
    FILE* system;
    system = fopen("system.img", "r+");
    if (system == 0) {
        printf("system.img not found\n");
        return;
    }

    // load map
    // map di 0x100
    char map[512];
    fseek(system, 512 * 0x100, SEEK_SET);
    for (i = 0; i < 512; i++) {
        map[i] = fgetc(system);
    }

    // load files
    // files di 0x101 & 0x102
    // size array 512 x 2 = 1024
    char file[1024];
    fseek(system, 512 * 0x101, SEEK_SET);
    for (i = 0; i < 1024; i++) {
        file[i] = fgetc(system);
    }

    // load sectors
    // sectors di 0x103
    char sector[512];
    fseek(system, 512 * 0x103, SEEK_SET);
    for (i = 0; i < 512; i++) {
        sector[i] = fgetc(system);
    }

    // find a free entry in files
    // tiap chunk ada 16 bytes
    for (i = 0; i < 1024; i = i + 16)
        if (file[i + 2] == 0) {
            break;
        }
    if (i == 1024) {
        printf("Not enough room in files\n");
        return;
    }
    int fileindex = i;

    // fill the name field with 00s first
    for (i = 0; i < 14; i++) {
        file[fileindex + i + 2] = 0x00;
    }
    // copy the name over
    for (i = 0; i < 14; i++) {
        if (argv[1][i] == 0) {
            break;
        }
        file[fileindex + i + 2] = argv[1][i];
    }

    // find a free entry in sectors
    for (i = 0; i < 32; i = i + 1)
        if (sector[i<<4] == 0) break;
    if (i == 32) {
        printf("Not enough room in sectors\n");
        return;
    }

    int sectorindex = i * 16;
    file[fileindex + 1] = i;
    file[fileindex] = 0xFF;

    // find free sectors and add them to the file
    int sectorcount = 0;
    while (!feof(loadedfile)) {
        if (sectorcount == 20) {
            printf("Not enough space in directory entry for file\n");
            return;
        }

        // find a free map entry
        for (i = 0; i < 256; i++) {
            if (map[i] == 0) {
                break;
            }
            if (i == 256) {
                printf("Not enough room for file\n");
                return;
            }
        }

        // mark the map entry as taken
        map[i] = 0xFF;

        // mark the sector in the directory entry
        sector[sectorindex] = i;
        sectorindex++;
        sectorcount++;

        // move to the sector and write to it
        fseek(system, i * 512, SEEK_SET);
        for (i = 0; i < 512; i++) {
            if (feof(loadedfile)) {
                fputc(0x0, system);
                break;
            } 
            else {
                char c = fgetc(loadedfile);
                fputc(c, system);
            }
        }
    }

    // write the map, files, and sectors back to the system image
    // map
    fseek(system, 512 * 0x100, SEEK_SET);
    for (i = 0; i < 512; i++) {
        fputc(map[i], system);
    }

    // files
    fseek(system, 512 * 0x101, SEEK_SET);
    for (i = 0; i < 1024; i++) {
        fputc(file[i], system);
    }

    // sectors
    fseek(system, 512 * 0x103, SEEK_SET);
    for (i = 0; i < 512; i++) {
        fputc(sector[i], system);
    }

    fclose(system);
    fclose(loadedfile);
}