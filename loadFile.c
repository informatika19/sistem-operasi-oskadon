// loadFile.c
// Michael Black, 2007

// Loads a file into the file system
// This should be compiled with gcc and run outside of the OS

#include <stdio.h>

int findFreeEntry(char* entries) {
    int i;
    for (i = 0; i < 64; i++) {
        if (entries[i * 16 + 1] == '\0') {
            return i;
        }
    }

    return -1;
}

int findFreeSector(char* map) {
    int i;
    for (i = 0; i < 0x100; i++) {
        if (map[i] == 0x00) {
            return i;
        }
    }

    return -1;
}

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
    // files di 0x102
    char file[512];
    fseek(system, 512 * 0x102, SEEK_SET);
    for (i = 0; i < 512; i++) {
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
    int entry = findFreeEntry(file);
    if (entry != -1) {
        int sectorCount = 0;
        while (!feof(loadedfile)) {
            int freeSector = findFreeSector(map);
            if (sector != -1) {
                fseek(loadedfile, sectorCount * 512, SEEK_SET);
                fseek(system, freeSector * 512, SEEK_SET);
                for (i = 0; i < 512; i++) {
                    if (!feof(loadedfile)) {
                        fputc(fgetc(loadedfile), system);
                    }
                    else {
                        fputc(0x00, system);
                    }
                }
                printf("%s diload ke sector %d\n", argv[1], freeSector);
                map[freeSector] = 0xFF;
                sector[entry * 16 + sectorCount] = freeSector;
                sectorCount += 1;
            }
            else {
                printf("Sector penuh\n");
                return;
            }
        }

        file[entry * 16] = 0xFF;

        // tulis nama file
        for (i = 0; argv[1][i] != '\0'; i++) {
            file[entry * 16 + 1 + i] = argv[1][i];
        }
        for (; i < 15; i++) {
            file[entry * 16 + 1 + i] = '\0';
        }

        // write map
        fseek(system, 512 * 0x100, SEEK_SET);
        for (i = 0; i < 512; i++) {
            fputc(map[i], system);
        }

        // write files
        fseek(system, 512 * 0x102, SEEK_SET);
        for (i = 0; i < 512; i++) {
            fputc(file[i], system);
        }

        // write sectors
        fseek(system, 512 * 0x103, SEEK_SET);
        for (i = 0; i < 512; i++) {
            fputc(sector[i], system);
        }
    }
    else {
        printf("Maximum number of files reached");
    }

    fclose(system);
    fclose(loadedfile);
}