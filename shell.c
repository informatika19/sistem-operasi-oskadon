
void shell();
void executecmd(char* cmd);
int modifiedstrcmp(char* a, char* b, int len,int start);
int ignoreSpace(char* cmd, int start);
void cd(char* param);
void ls(char* param);
void cat(char* param);
void ln(char* param);


void shell(){
    char cmd[180];
    //char cmd1[180]; //history
    //char cmd2[180]; //history
    //char cmd3[180]; //history
    //int HistHEAD = 1;
    printString("Executing Shell...\n");
    while(1){
        printString("$ ");
        readString(cmd);
        //printString(cmd);
        executecmd(cmd);
        //strcpy(cmd2,cmd3);
        //strcpy(cmd1,cmd2);
        //strcpy(cmd,cmd1);
        //clear(cmd,180);
    }

}

void executecmd(char* cmd){
    int cmdIndex = 0;
    if(modifiedstrcmp(cmd,"cd",2,cmdIndex)){
        cmdIndex += 2;
        cmdIndex = ignoreSpace(cmd,cmdIndex);
        cd(&cmd[cmdIndex]);
    }else if(modifiedstrcmp(cmd,"ls",2,cmdIndex)){
        cmdIndex += 2;
        cmdIndex = ignoreSpace(cmd,cmdIndex);
        ls(&cmd[cmdIndex]);
    }else if(modifiedstrcmp(cmd,"cat",3,cmdIndex)){
        cmdIndex += 3;
        cmdIndex = ignoreSpace(cmd,cmdIndex);
        cat(&cmd[cmdIndex]);
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

void cd(char* param){
    printString("calling cd with parar : ");
    printString(param);
    printString("\n");
}
void ls(char* param){
    printString("calling ls with parar : ");
    printString(param);
    printString("\n");
}
void cat(char* param){
    printString("calling cat with parar : ");
    printString(param);
    printString("\n");
}
void ln(char* param){
    printString("calling ln with parar : ");
    printString(param);
    printString("\n");
}