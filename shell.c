#define bool unsigned char
#define true 1
#define false 0

void shell();
void executecmd(char* cmd,char* currDir);
int modifiedstrcmp(char* a, char* b, int len,int start);
int ignoreSpace(char* cmd, int start);
void cd(char* param,char* currDir);
void ls(char* param,char* currDir);
void cat(char* param,char* currDir);
void ln(char* param,char* currDir);


void shell(){
    char cmd[180];
    char* currDir;  // directory sekarang
    //char cmd1[180]; //history
    //char cmd2[180]; //history
    //char cmd3[180]; //history
    //int HistHEAD = 1;

    // Inisialisasi
    *currDir = 0xFF;    //inisialisasi sebagai root;
    printString("Executing Shell...\n");
    while(1){
        printString("$ ");
        readString(cmd,currDir);
        //printString(cmd);
        executecmd(cmd,currDir);
        //strcpy(cmd2,cmd3);
        //strcpy(cmd1,cmd2);
        //strcpy(cmd,cmd1);
        //clear(cmd,180);
    }

}

void executecmd(char* cmd, char* currDir){
    int cmdIndex = 0;
    if(modifiedstrcmp(cmd,"cd",2,cmdIndex)){
        cmdIndex += 2;
        cmdIndex = ignoreSpace(cmd,cmdIndex);
        cd(&cmd[cmdIndex],currDir);
    }else if(modifiedstrcmp(cmd,"ls",2,cmdIndex)){
        cmdIndex += 2;
        cmdIndex = ignoreSpace(cmd,cmdIndex);
        ls(&cmd[cmdIndex],currDir);
    }else if(modifiedstrcmp(cmd,"cat",3,cmdIndex)){
        cmdIndex += 3;
        cmdIndex = ignoreSpace(cmd,cmdIndex);
        cat(&cmd[cmdIndex],currDir);
    }else{
        cmdIndex = ignoreSpace(cmd,cmdIndex);
        printString("invalid command\n");
    }

}

int modifiedstrcmp(char* a, char* b, int len,int start){ //compare untuk beberapa char pertama aja dimulai dari index sesuati *buat shell
    int i;
    i = 0;
    while(i < len){
        if(a[start + i] != b[i]){
            return 0;
        }
        i++;
    }
    return 1;
}

int ignoreSpace(char* cmd, int start){ //return new index
    while(cmd[start] == 0x20){
        start++;
    }
    return start;
}

void cd(char* param,char* currDirIdx) {
    // KAMUS
    char files[1024];
    char currFlName[128];
    char* foundIdx;
    bool found;
    int i,j;

    printString("calling cd with parar : ");
    printString(param);
    printString("\n");

    readSector(files, 0x101);
    readSector(files+512, 0x102);

    
    i = 0;
    j = 0;
    while (param[i] != '\0') 
    {
        currFlName[j] = param[i];
        if (currFlName[j] == '/' || currFlName[j] == '\0') {
            currFlName[j] = '\0';
            // Cek apakah nama folder valid (tidak boleh kosong)
            if (strcmp(currFlName,".")) 
            {
                // Do Nothing, di curr dir yang sama
            } 
            else if (strcmp(currFlName,"..")) 
            {   // Back to parent
                if (*currDirIdx != 0xFF) {    // Bukan di root
                    // Cari parent folder ini
                    *currDirIdx = files[(*currDirIdx)*16];
                }
                // kalau di root do nothing aja
            } 
            else if (strcmp(currFlName,"")) 
            {
                printString("Error: Nama folder tidak valid");
                return;
            } 
            else    // default, sebuah nama folder
            {        
                found = isFlExist(files,currDirIdx,currFlName,true,foundIdx);
                if (!found) { // folder tidak ada
                    printString("Error: Folder tidak ditemukan\n");
                    return;
                }
                // ditemukan
                *currDirIdx = *foundIdx;
            }
            j = 0;
        } else {
            j++;
        }

        if (param[i] != '\0') {
            i++;
        }
    } 


    // // Cek apakah nama file valid (tidak boleh kosong)
    // if (strcmp(currFlName,"")) {
    //     printString("Nama file tidak valid");
    //     *result = -5;
    //     return;
    // }

    // // 3. Cek file ada
    // found = isFlExist(files,*currDir,currFlName,false,foundIdx);
    // if (!found) {
    //     printString("File tidak ditemukan\n");
    //     return;
    // }


}
void ls(char* param,char* currDir){
    printString("calling ls with parar : ");
    printString(param);
    printString("\n");
}
void cat(char* param,char* currDir){
    printString("calling cat with parar : ");
    printString(param);
    printString("\n");
}
void ln(char* param,char* currDir){
    printString("calling ln with parar : ");
    printString(param);
    printString("\n");
}