#define bool unsigned char
#define true 1
#define false 0

void shell();
void executecmd(char* cmd,int* currDirIdx);
int modifiedstrcmp(char* a, char* b, int len,int start);
int ignoreSpace(char* cmd, int start);
void printCurrDirName(int currDirIdx);
void cd(char* param,int* currDirIdx);
void ls(char* param,int* currDirIdx);
void cat(char* param,int* currDirIdx);
void ln(char* param,int* currDirIdx);

void consdot(char* dest, char* add);
char* append(char* first, char* last);

void shell(){
    char cmd[180];
    int* currDirIdx;  // directory sekarang
    //char cmd1[180]; //history
    //char cmd2[180]; //history
    //char cmd3[180]; //history
    //int HistHEAD = 1;

    // Inisialisasi
    printString("Executing Shell...\n");
    *currDirIdx = 0xFF;    //inisialisasi sebagai root;
    while(1){
        printCurrDirName(*currDirIdx);
        printString(" $ ");
        readString(cmd);
        executecmd(cmd,currDirIdx);
        //strcpy(cmd2,cmd3);
        //strcpy(cmd1,cmd2);
        //strcpy(cmd,cmd1);
        //clear(cmd,180);
    }

}

void executecmd(char* cmd, int* currDirIdx){
    int cmdIndex = 0;
    if(modifiedstrcmp(cmd,"cd",2,cmdIndex)){
        cmdIndex += 2;
        cmdIndex = ignoreSpace(cmd,cmdIndex);
        cd(&cmd[cmdIndex],currDirIdx);
    }else if(modifiedstrcmp(cmd,"ls",2,cmdIndex)){
        cmdIndex += 2;
        cmdIndex = ignoreSpace(cmd,cmdIndex);
        ls(&cmd[cmdIndex],currDirIdx);
    }else if(modifiedstrcmp(cmd,"cat",3,cmdIndex)){
        cmdIndex += 3;
        cmdIndex = ignoreSpace(cmd,cmdIndex);
        cat(&cmd[cmdIndex],currDirIdx);
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
    while(cmd[start] == ' '){
        start++;
    }
    return start;
}

void printCurrDirName(int currDirIdx) {
    char files[1024];
    int filesIdx;
    int i;
    char buffName[256];
    char currFlName[15];

    buffName[0] = '\0';
    readSector(files, 0x101);
    readSector(files+512, 0x102);  

    filesIdx = currDirIdx;
    

    if (filesIdx == 0xFF) { // udah root
        printString("/root");
        return;
    } else {
        getFlName(files,filesIdx,currFlName);
        filesIdx = files[filesIdx*16];
        printCurrDirName(filesIdx);

        printString("/");
        printString(currFlName);

    }

}


void cd(char* param,int* currDirIdx) {
    // KAMUS
    char files[1024];
    char currFlName[128];
    int temp;
    int* foundIdx;
    bool found;
    int i,j;

    printString("calling cd with parar : ");
    printString(param);
    printString("\n");

    readSector(files, 0x101);
    readSector(files+512, 0x102);

    
    i = 0;
    j = 0;
    if (param[i] = '/') {   // dari root
        *currDirIdx = 0xFF;
        i++;  
    }
    
    while (i <= strlen(param))
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
                printString("Error: Nama folder tidak valid\n");
                return;
            } 
            else    // default, sebuah nama folder
            {      
                temp = *currDirIdx;
                found = isFlExist(files,*currDirIdx,currFlName,true,foundIdx);
                
                if (!found) { // folder tidak ada
                    *currDirIdx = temp;
                    printString("Error: Folder tidak ditemukan\n");
                    return;
                } else {
                    // ditemukan
                    *currDirIdx = *foundIdx;
                }
            }
            j = 0;
        } else {
            j++;
        }

        i++;

        
             
    }


    // printString("Cd berhasil");


}
void ls(char* param,int* currDirIdx); {
    printString("calling ls with parar : ");
    printString("\n");
}
void cat(char* param, int* currDirIdx)  {
    char buffer[1024];
    int result;
    result = 1;
    readFile(buffer, param, result, currDirIdx);
    if (result == -1) {
        printString("File tidak ditemukan\n");
    }
    printString(buffer);
    printString('\n');
}
// ln [-fs] [-L|-P] source_file target_file
// example : ln test4.txt text6.txt made new file text6.txt that linked with test4.txt
void ln(char* param,int* currDirIdx){
    int i,j;
    char sourceFileName[180];
    char targetFileName[180];
    char currFlName[128];
    char maps[512];
    char files[1024];
    char* pathSource;
    char* pathTarget;
    char currParentIdx;
    char parentIndex;
    int sectNum, sectPos, sectIdx, fileSourceIdx;
    int found;
    char* foundIdx;
    int dirNum,dirIdx;
    int sectorSourcefileIdx;
    parentIndex  = *currDirIdx;
    i = 0;
    while(param[i] != ' '){
        sourceFileName[i] = param[i];
    }
    i = ignoreSpace(param,i);
    j = 0;
    while(param[i+j] != 0x20 || param[i+j] != 0x0){
        targetFileName[j] = param[i+j];
    }
    pathSource = *sourceFileName; // pathSource = ./folder/file ; currDirIdx = 0xFF
    pathTarget = *targetFileName;
    //below is from readFile from kernel.c
    
    // 0. inisialisasi
    //readSector(maps, 0x100);
    readSector(files, 0x101);
    readSector(files + 512, 0x102);

    // 1. cari folder dan file
    i = 0;
    j = 0;
    if (currDirIdx[i] == '/') { // root dir 
        currParentIdx = 0xFF;
        i += 1;
    } else if (pathSource[i] == '.' && pathSource[i+1] == '/') { // current dir (spesifik)
        currParentIdx = parentIndex;
        i += 2;
    } else {    // current dir (default)
        currParentIdx = parentIndex;
    }

    // 2. Cari pathSource sampe ketemu bagian file
    while (pathSource[i] != '\0') {
        currFlName[j] = pathSource[i];
        if (currFlName[j] == '/') {
            currFlName[j] = '\0';
            // Cek apakah nama folder valid (tidak boleh kosong)
            if (strcmp(currFlName,"")) {
                printString("Nama folder tidak valid");
                //*result = -5;
                return;
            }
             
            // Cek apakah sudah tersedia folder yang sama
            found = isFlExist(files,currParentIdx,currFlName,true,foundIdx);
            if (found) {
                printString("Folder sudah ada\n");
                currParentIdx = *foundIdx;
                // printString(currParentIdx);
            } else {
                printString("Folder tidak ditemukan\n");
                //*result = -1;
                return;
            }
            j = 0;
        } else {
            j++;
        }
        i++;
    }
    // Cek apakah nama file valid (tidak boleh kosong)
    if (strcmp(currFlName,"")) {
        printString("Nama file tidak valid");
        //*result = -5;
        return;
    }

    // 3. Cek file ada
    found = isFlExist(files,currParentIdx,currFlName,false,foundIdx);
    if (!found) {
        printString("File tidak ditemukan\n");
        return;
    }

    printString("File ditemukan\n");
    fileSourceIdx = *foundIdx;

    //yang dibawah sekarang copy dari write file
    // cari dir yang kosong
    dirIdx = foundEmptyDir(files);
    if (dirIdx == -1) {
        printString("Tidak cukup entri di files\n");
        //*sectors = -2;
        return;
    }
    dirNum = dirIdx*16;
    // 5. Buat File
    sectorSourcefileIdx = files[fileSourceIdx*16+1];
    writeDir(files,dirNum,currParentIdx,sectorSourcefileIdx,currFlName);
}

void consdot(char* dest, char* add) {
    int lenAdd;
    int lenDest;
    int i, j;

    lenAdd = strlen(add);
    lenDest = strlen(dest);

    // printString("\nm1 ");
    // printString(dest);
    // printString(" m1");
    // printString("\nm2 ");
    // printString(add);
    // printString(" m2\n");
    i = lenDest-1;
    while (i >= 0)
    {
        dest[i+lenAdd] = dest[i];
        i--;
    }
    
    i = 0;
    while (i < lenAdd)
    {
        
        dest[i] = add[i];
        i++;
    }
    dest[lenDest+lenAdd] = '\0';
    // printString(dest);
}




char* append(char* first, char* last){
    //const int lenResult = strlen(first) + strlen(last);
    char result[256]; 
    int i,j;

    i = 0;
    for(i;i<strlen(first);i++){
        result[i] = first[i];
    }
    j = 0;
    for(j;j<strlen(last);j++){
        result[i+j] = last[j];
    }
    result[i+j] = '\0';
    return result;
}