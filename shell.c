#include "module/boolean.h"
#include "module/fileIO.h"
#include "module/folderIO.h"
#include "module/math.h"
#include "module/text.h"
#include "module/sector.h"

void executecmd(char* cmd,char* cmd1,char* cmd2,char* cmd3,int* currDirIdx);
int modifiedstrcmp(char* a, char* b, int len,int start);
int ignoreSpace(char* cmd, int start);
void printCurrDirName(int currDirIdx);
void cd(char* param,int* currDirIdx);
void ls(int* currDirIdx);
void cat(char* param,int* currDirIdx);
// void ln(char* param,int currDirIdx);
void help();
void prev(char* cmd ,int* currDirIdx, char* cmd1 ,char* cmd2 ,char* cmd3);

int main(){
    char buffCurrDirIdx[512];
    char cmd[180],cmd1[180],cmd2[180],cmd3[180];
    int* currDirIdx;  // directory sekarang
    int i;
    cmd1[0] = 0x00;
    cmd2[0] = 0x00;
    cmd3[0] = 0x00;
    // Inisialisasi
    printString("Executing Shell...\n");
    clear(buffCurrDirIdx,512);
    readSector(buffCurrDirIdx,800);
    *currDirIdx = buffCurrDirIdx[0];

    // *currDirIdx = 0xFF;    //inisialisasi sebagai root;
    while(1){
        printCurrDirName(*currDirIdx);
        printString(" $ ");
        readString(cmd);
        executecmd(cmd,cmd1,cmd2,cmd3,currDirIdx);
        i = 0;
        i = ignoreSpace(cmd,i);
        if(!modifiedstrcmp(cmd,"prev",4,i)){
            strcpy(cmd3,cmd2);
            strcpy(cmd2,cmd1);
            strcpy(cmd1,cmd);    
        }
    }

}

void executecmd(char* cmd,char* cmd1,char* cmd2,char* cmd3, int* currDirIdx){
    char buffCurrDirIdx[512];
    char buffParam[512];
    int cmdIndex = 0;
    int dump;
    int tempcurrDirIdx = *currDirIdx;

    clear(buffCurrDirIdx,512);
    clear(buffParam,512);

    // EKSEKUSI GLOBAL
    cmdIndex = ignoreSpace(cmd,cmdIndex);
    if(modifiedstrcmp(cmd,"cd",2,cmdIndex)){
        cmdIndex += 2;
        cmdIndex = ignoreSpace(cmd,cmdIndex);
        cd(&cmd[cmdIndex],currDirIdx);
    }else if(modifiedstrcmp(cmd,"ls",2,cmdIndex)){
        cmdIndex += 2;
        cmdIndex = ignoreSpace(cmd,cmdIndex);
        ls(currDirIdx);
        *currDirIdx = tempcurrDirIdx;
    }else if(modifiedstrcmp(cmd,"cat",3,cmdIndex)){
        cmdIndex += 3;
        cmdIndex = ignoreSpace(cmd,cmdIndex);

        strcpy(buffCurrDirIdx,currDirIdx);
        strcpy(buffParam,&cmd[cmdIndex]);

        writeSector(buffCurrDirIdx,800);
        writeSector(buffParam,801);

        interrupt(0x21, 0xFF06, "bin/cat", 0x2000, &dump);

        // cat(&cmd[cmdIndex],currDirIdx);
        // *currDirIdx = tempcurrDirIdx;
    }else if (modifiedstrcmp(cmd,"ln",2,cmdIndex)){
        // Currenly not avalible
        cmdIndex += 2;
        cmdIndex = ignoreSpace(cmd,cmdIndex);
        
        strcpy(buffCurrDirIdx,currDirIdx);
        strcpy(buffParam,&cmd[cmdIndex]);

        writeSector(buffCurrDirIdx,800);
        writeSector(buffParam,801);

        interrupt(0x21, 0xFF06, "bin/ln", 0x2000, &dump);
        // ln(&cmd[cmdIndex],*currDirIdx);
        // *currDirIdx = tempcurrDirIdx;
    }else if(modifiedstrcmp(cmd,"help",4,cmdIndex)){
    	cmdIndex += 4;	
    	cmdIndex = ignoreSpace(cmd,cmdIndex);
	    help();
        *currDirIdx = tempcurrDirIdx;
    }else if(modifiedstrcmp(cmd,"prev",4,cmdIndex)){
    	cmdIndex += 4;	
    	cmdIndex = ignoreSpace(cmd,cmdIndex);
	    prev(&cmd[cmdIndex],currDirIdx,cmd1,cmd2,cmd3);
    } else if (modifiedstrcmp(cmd,"cek",3,cmdIndex)) {
        cmdIndex += 3;
        cmdIndex = ignoreSpace(cmd,cmdIndex);

        strcpy(buffCurrDirIdx,currDirIdx);
        strcpy(buffParam,&cmd[cmdIndex]);

        writeSector(buffCurrDirIdx,800);
        writeSector(buffParam,801);
        interrupt(0x21, 0xFF06, "bin/cek", 0x2000, &dump);
        // executeProgram("cek", 0x2000, &dump, 0xFF);
    } else if (modifiedstrcmp(cmd,"mv",2,cmdIndex)) {
        cmdIndex += 2;
        cmdIndex = ignoreSpace(cmd,cmdIndex);

        strcpy(buffCurrDirIdx,currDirIdx);
        strcpy(buffParam,&cmd[cmdIndex]);

        writeSector(buffCurrDirIdx,800);
        writeSector(buffParam,801);
        interrupt(0x21, 0xFF06, "bin/mv", 0x2000, &dump);
        // executeProgram("cek", 0x2000, &dump, 0xFF);
    } else if (modifiedstrcmp(cmd,"./",2,cmdIndex)) {
        // cmdIndex += 2;
        // cmdIndex = ignoreSpace(cmd,cmdIndex);

        strcpy(buffCurrDirIdx,currDirIdx);
        strcpy(buffParam,&cmd[cmdIndex]);

        writeSector(buffCurrDirIdx,800);
        writeSector(buffParam,801);
        interrupt(0x21, 0xFF06, "bin/run", 0x2000, &dump);
        // executeProgram("cek", 0x2000, &dump, 0xFF);
    } else {
        //cmdIndex = ignoreSpace(cmd,cmdIndex);
        printString("'");
        printString(cmd);
        printString("' is not recognized, type 'help' to list all valid command \n");
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

    readSector(files, 0x101);
    readSector(files+512, 0x102);

    
    i = 0;
    j = 0;
    if (param[i] == '/') {   // dari root
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

}

void ls(int* currDirIdx) {
    char dir[512*2];
    int i;
    int parentIdx;

    parentIdx = *currDirIdx;

    // read dir & file
    readSector(dir, 0x101);
    readSector(dir + 512, 0x102);

    // print dir
    i = 0;
    while (i < 32) {
        if (dir[i * 16] == parentIdx && dir[i * 16 + 1] != 0x00 && dir[i * 16 + 2] != 0x00) {
            if (dir[i * 16 + 1] == 0xFF) {
                printString("*");
            } else {
                printString(" ");
            }

            printString(dir+(i * 16 + 2));
            printString("\n\r");
        }
        i++;
    }
}


void help(){
	printString("help  listing all possible commands\n");
	printString("cd    changes the current directory\n");
	printString("ls    lists all files in the current directory\n");
	printString("cat   view contain of given file\n");
	printString("ln    create a hard link or a symbolic link to an existing file or directory\n");
    printString("prev  execute previous command (up to 3 command)\n"); 
}

void prev(char* cmd ,int* currDirIdx, char* cmd1 ,char* cmd2 ,char* cmd3){
    int i = 0;
    if(cmd[i] == '1'){
        printString("\nexecuting ");
        printString(cmd1);
        printString("\n");
        executecmd(cmd1,cmd1,cmd2,cmd3,currDirIdx);
    }else if(cmd[i] == '2'){
        printString("\nexecuting ");
        printString(cmd2);
        printString("\n");
        executecmd(cmd2,cmd1,cmd2,cmd3,currDirIdx);
    }else if(cmd[i] == '3'){
        printString("\nexecuting ");
        printString(cmd3);
        printString("\n");
        executecmd(cmd3,cmd1,cmd2,cmd3,currDirIdx);
    }else{
        printString("command out of bound\n");
    }
}


