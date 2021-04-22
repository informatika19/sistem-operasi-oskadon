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