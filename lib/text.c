#include "text.h"

int strlen(char* buff) {
    int i = 0;
    while (buff[i] != '\0') {
        i++;
    }
    return i;
}

int strcmp(char* a, char* b) {
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

void strcpy(char* dest, char* src) {
    int i;

    i = 0;
    for (i; i < strlen(src); i++) {
        dest[i] = src[i];
    }

    src[i] = 0x00;
    dest[i] = 0x00;
}

void clear(char* buffer, int length) {
    int i = 0;
    while (i < length) {
        buffer[i] = 0x0;
        i++;
    }
}

void printString(char* string) {
    int AL = 0x0E00;
    int i = 0;
    while (string[i] != '\0') {
        interrupt(0x10, AL+string[i], 0x000F, 0, 0);

        if (string[i] == '\n') {
            interrupt(0x10, AL+0xd, 0x000F, 0, 0);
        }

        i++;
    }
}

void readString(char* string) {
    int INT_10H = 0x10;
    int INT_16H = 0x16;

    int AL = 0x0E00;
    int loop = 1;
    int i = 0;
    int ascii;

    while (loop)
    {
        ascii = interrupt(INT_16H,0,0,0,0);
        if (ascii == '\r') {
            string[i] = '\0';
            interrupt(INT_10H,AL+'\n',0,0,0);
            interrupt(INT_10H,AL+'\r',0,0,0);
            loop = 0;
        } else if (ascii == '\b') {
            if (i > 0 && mod(i,80) != 0) {
                interrupt(INT_10H,AL+'\b',0,0,0);
                interrupt(INT_10H,AL+'\0',0,0,0);
                interrupt(INT_10H,AL+'\b',0,0,0);
                i--;
            }
        } else {
            string[i] = ascii;
            interrupt(INT_10H,AL+ascii,0,0,0);
            i++;
        }      
    }
}


int ignoreSpace(char* cmd, int start){ //return new index
    while(cmd[start] == ' '){
        start++;
    }
    return start;
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


/***


bool isFirstLetter(char* first, char* compare) {
    // ngecek apakah char first adalah huruf pertama string
    int i;

    for (i = 0; i < 14; i++) {
        if (first[i] != compare[i]) {
            return false;
        }
    }

    return true;
}
***/