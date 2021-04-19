#include "module/boolean.h"
#include "module/fileIO.h"
#include "module/folderIO.h"
#include "module/math.h"
#include "module/text.h"

int resolvePath(int from, char *path);
void inputWithComplete(char *string);
void ls(char parentIndex);
void cd(char *fileName);
void mkdir(char* folderName);
void ln(char parentIndex, char* fileName1, char* fileName2);
int strcmp(char* strA, char* strB);
int strcmpn(char* strA, char* strB, int n);
void initHistory();
void updateHistory(char* input);

int curdir, dirBack, dirChange, itrDirName;
char curDirName[128], directoryBuffer[1024], history[3][64];
curdir = 0xFF;

int main(){
    char arg1[14], arg2[14], directoryPath[512], directoryDestination[14], fileContent[512*16];
	char* input;
	int i, j, k, success, isSuccess, isSuccess2; 
    // int isSuccess;
	curdir = 0xFF;
	itrDirName = 0;
	dirChange = 0;
	dirBack = 0;
	
    // Initialize current directory name 
    i = 0;
    while(i<512){
        if(i<128){
            curDirName[i] = '\0';
        }
        directoryPath[i] = 0x00;
        i++;
    }
    // Load files sector at 0x101 and 0x102
    interrupt(0x21, 0x2, directoryBuffer, 0x101, 0);
    interrupt(0x21, 0x2, directoryBuffer+512, 0x102, 0);
    initHistory();
	i = 0;
    while(1){
        do{
            // output Root to shell
            interrupt(0x21, 0x00, "shell@wetheclown", 0, 0);

            // Load current directory name next to "Root"
            interrupt(0x21, 0x00, curDirName, 0, 0);
			interrupt(0x21, 0x00, ">", 0, 0);
            // Ask for input from user on shell
			//interrupt(0x21, 0x01, input, 1, 0);
            inputWithComplete(input);
        } while(strcmp(input, ""));
        
        updateHistory(input);
        interrupt(0x21, 0x00, "\r\n", 0, 0);

        // List of Command
        if(strcmp(input, "ls\r\n")){
            ls(curdir);
        } else if(strcmpn(input, "ls ",3)){
            int parentIndex = resolvePath(curdir, input+3);
            if(parentIndex<0){
                interrupt(0x21, 0x00, "File atau Direktori tidak ditemukan\r\n", 0, 0);
            }else{
                ls(parentIndex);
            }
        }else if(strcmpn(input, "cat ",4)) {
            i = 4;
            while (i < 18) {
                if (input[i] == '\0' || input[i] == '\r') {
                    break;
                }else{
                    // Mengambil argument sebenarnya dari cat
                    arg1[i - 4] = input[i];
                } 
                i++;
            }
            // Mengosongkan argumen bagian paling belakang apabila panjang argumen cat tidak sampai 15
            while (i < 18) {
                arg1[i-4] = '\0';
                i++;
            }
            directoryPath[0] = curdir;
            for (i = 0; i < 14; i++) {
                directoryPath[i+1] = arg1[i];
            }
            // Read sector and fill to directory path
            //interrupt(0x21, 0x03, directoryPath, 512, 0);
            //interrupt(0x21, 0x06, "cat", 0x2000, &success);
            // Gatau mau ngapain ? :)
            for (i = 0; i < 512*16; i++) {
		        fileContent[i] = 0x0;
	        }
            interrupt(0x21, 0x04 + curdir*256, fileContent, arg1, &isSuccess);
            
            if (isSuccess==1){
                interrupt(0x21, 0x00, fileContent, 0, 0);
                interrupt(0x21, 0x00, "\r\n\0", 0, 0);
            } else if(isSuccess==-1) {
                interrupt(0x21, 0x00, "File tidak ditemukan dalam direktori ini\r\n\0", 0, 0);
            }
            
            // Load ulang files sector at 0x101 and 0x102
            interrupt(0x21, 0x2, directoryBuffer, 0x101, 0);
            interrupt(0x21, 0x2, directoryBuffer+512, 0x102, 0);
        } else if(strcmpn(input, "cd ", 3)){
            cd(input+3);
        } else if(strcmpn(input, "mkdir ", 6)){
            mkdir(input+6);
            interrupt(0x21, 0x3, directoryBuffer, 0x101, 0);
            interrupt(0x21, 0x3, directoryBuffer+512, 0x102, 0);
        } else if(strcmpn(input, "ln ", 3)){
            // Kosongkan arg1 dan arg2 untuk diisi nama file
            for(i=0; i<14; i++){
                arg1[i] = '\0';
                arg2[i] = '\0';
            }
            i = 3; // iterator input
            j = 0; // panjang nama file pertama
            while(i<17){
                if (input[i] == '\0' || input[i]==' ') {
                    break;            isSuccess = 0;
            isSuccess2 = 0;
                }else{
                    // Mengambil nama file pertama
                    arg1[i-3] = input[i];
                } 
                i++;
                j++;
            }
            i++; // asumsi pasti ada spasi diantara nama file pertama dan kedua
            while(i<32){
                if (input[i] == '\0' || input[i] == '\r') {
                    break;
                }else{
                    // Mengambil nama file kedua
                    arg2[i-j-4] = input[i];
                } 
                i++;
            }
            ln(curdir, arg1, arg2);
        } else {    
            interrupt(0x21, 0x00, "Command ini tidak tersedia\r\n\0", 0, 0);
        }
    }

    return 0;
}

void initHistory(){
    int i,j;
    for(i=0; i<3; i++){
        for(j=0; j<64; j++){
            history[i][j] = 0;
        }
    }
}
void updateHistory(char* input){
    int i,j;
    for(i=2; i>0; i--){
        for(j=0; j<64; j++){
            history[i][j] = history[i-1][j];
        }
    }
    for(j=0; j<64; j++){
        history[0][j] = 0;
    }
    for(j=0; input[j]!='\r'; j++){
        history[0][j] = input[j];
    }
}

int resolvePath(int from, char *path){
    //Mengembalikan id file/folder yang diperoleh melalui path dari id from
    //Mengembalikan -1 jika tidak ada
    int id, found, len;
    found = -1;
    for(len=0; path[len]!='/' && path[len]!='\r'; len++){}
    
    // Mencari folder/file yang namanya sesuai
    for(id=0; id<64; id++){
        if(directoryBuffer[id*16] == from){
            if(stringLength(directoryBuffer + (id*16 + 2), 14) == len){
                if(strcmpn(directoryBuffer + (id*16 + 2), path, len)){
                    found = id;
                }
            }
        }
    }
    // Mengecek apakah . atau ..
    if(len==2 && path[0] == '.' && path[1] == '.'){
        found = directoryBuffer[from*16];
    }else if(len==1 && path[0] == '.'){
        found = from;
    }
    if(found<0){
        return -1;
    }else if(path[len]=='\r'){
        return found;
    //path[len] pasti bernilai '/', hanya perlu mengecek apakah folder/file
    }else if(directoryBuffer[id*16+1]==0xFF){
        return resolvePath(found, path + len);
    }else{
        return -1;
    }
}


void inputWithComplete(char *string){
    // initialize variable
    int i = 0; //string's length
    int j = 0; //input position
    int k = 0; //for loop index
    int idx, nxt, found, len, hist;
    char c = '\0';
    idx = 0;
    hist = -1;
    while(1){
        // Wait for keypress and read character from keypress/keyboard
        c = interrupt(0x16, 0, 0, 0, 0);
        // Display input from keyboard
        if(c == '{' || c == '}'){
            if(c=='{' && hist<2){hist++;}
            if(c=='}' && hist>0){hist--;}
            while(j>0){
                interrupt(0x10, 0xE*256+8,0,0,0);
                j--;
            }
            for(k=0; k<i; k++){
                interrupt(0x10, 0xE*256+' ',0,0,0);
            }
            for(k=0; k<i; k++){
                interrupt(0x10, 0xE*256+8,0,0,0);
                string[k] = 0;
            }
            i=0;
            while(history[hist][i] != 0){
                interrupt(0x10, 0xE*256+history[hist][i],0,0,0);
                string[i] = history[hist][i];
                i++;
                j++;
            }
        }else if(i>=3 && string[0]=='c' && string[1]=='d' && string[2] == ' ' && c==','){
            nxt = idx;
            found = 0;
            do{
                nxt++;
                if(nxt==64){
                    nxt=0;
                }
                found = (directoryBuffer[16*nxt]==curdir) && (directoryBuffer[16*nxt+1]==0xFF);
            }while(!found && nxt!=idx);
            if(found){
                //Hapus keseluruhan string
                while(j>3){
                    interrupt(0x10, 0xE*256+8,0,0,0);
                    j--;
                }
                for(k=3; k<i; k++){
                    interrupt(0x10, 0xE*256+' ',0,0,0);
                }
                for(k=3; k<i; k++){
                    interrupt(0x10, 0xE*256+8,0,0,0);
                    string[k] = 0;
                }
                //Tulis autocomplete yang diperoleh
                i=3;
                while(i<17 && directoryBuffer[16*nxt+i-1]!=0){
                    interrupt(0x10, 0xE*256 + directoryBuffer[16*nxt+i-1],0,0,0);
                    string[i] = directoryBuffer[16*nxt+i-1];
                    i++;
                }
                //Update variabel
                j=i;
                idx = nxt;
            }
        }else if((i>=3 && strcmpn(string, "ls ", 3)
                ||i>=3 && strcmpn(string, "ln ", 3)
                ||i>=4 && strcmpn(string, "cat ", 4)) 
                && c==','){
            if(i>=4 && strcmpn(string, "cat ", 4)){
                len = 4;
            }else{
                len = 3;
            }
            nxt = idx;
            found = 0;
            do{
                nxt++;
                if(nxt==64){
                    nxt=0;
                }
                found = (directoryBuffer[16*nxt]==curdir) && (directoryBuffer[16*nxt+1]!=0xFF);
            }while(!found && nxt!=idx);
            if(found){
                //Hapus keseluruhan string
                while(j>len){
                    interrupt(0x10, 0xE*256+8,0,0,0);
                    j--;
                }
                for(k=len; k<i; k++){
                    interrupt(0x10, 0xE*256+' ',0,0,0);
                }
                for(k=len; k<i; k++){
                    interrupt(0x10, 0xE*256+8,0,0,0);
                    string[k] = 0;
                }
                //Tulis autocomplete yang diperoleh
                i=len;
                while(i<17 && directoryBuffer[16*nxt+2+i-len]!=0){
                    interrupt(0x10, 0xE*256 + directoryBuffer[16*nxt+2+i-len],0,0,0);
                    string[i] = directoryBuffer[16*nxt+2+i-len];
                    i++;
                }
                //Update variabel
                j=i;
                idx = nxt;
            }
        }else if(c==0xD){
            interrupt(0x10, 0x0E*256+c, 0, 0, 0);
            // If enter
            // Add Carriage Return, so the next print will move to start of the newline
            string[i] = '\r';
            // Add Newline
            string[i+1] = '\n';
            // Add EOF
            string[i+2] = '\0';
            break;
        } else if(c==0x8){
            // If backspace
            if(j > 0){//i>0
                // Check if there is any char input
                // delete by change the latest input char display to spaces
                char x;
                interrupt(0x10, 0xE*256+c,0,0,0);
                for(k=j; k<i; k++){
                    string[k-1] = string[k];
                    x = string[k-1];
                    interrupt(0x10, 0xE*256+x,0,0,0);
                }
                string[i-1] = ' ';
                x = ' ';
                interrupt(0x10, 0xE*256+x,0,0,0);
                for(k=i; k>=j; k--){
                    interrupt(0x10, 0xE*256+c,0,0,0);
                }
                j--;
                i--;
            }
        } else if (c == '<'){
            // Periksa bahwa inputnya left arrow,
            if(j>0){
                char back = 0x8;
                interrupt(0x10, 0xE*256+back,0,0,0);
                j--;
            }
        } else if (c == '>'){
            // Periksa bahwa inputnya shift+.
            if(j<i){
                char x = string[j];
                interrupt(0x10, 0xE*256+x,0,0,0);
                j++;
            }
        }else{
            // Char input
            // If user press special function key, it will reas as 0x0
            for(k=i; k>j; k--){
                string[k] = string[k-1];
            }
            string[j] = c;
            i++;
            for(k=j; k<i; k++){
                c = string[k];
                interrupt(0x10, 0xE*256+c,0,0,0);
            }
            j++;
            c = 0x8;
            for(k=i; k>j; k--){
                interrupt(0x10, 0xE*256+c,0,0,0);
            }
        }
    }
}


void ls(char parentIndex){
    // Initialization
    int id, nameLength, count;
    // Jika parentIndex bukan folder tapi file
    id = parentIndex;
    if(id!= 0xFF && directoryBuffer[id*16+1] != 0xFF){
        interrupt(0x21, 0x00, "(file) ", 0, 0);
        for(nameLength=0; nameLength<14; nameLength++){
            if(directoryBuffer[id*16 + 2 + nameLength] == '\0'){
                break;
            }
            interrupt(0x10, 0xE00 + directoryBuffer[id*16 + 2 + nameLength], 0, 0, 0);
        }
        interrupt(0x21, 0x00, "\r\n\0", 0, 0);
        return;
    }
    // Jika parentIndex ialah folder
    count = 0;
    // Check for all reserved memory for files
    // Since 32 files/directory can stored in each sector so there can be up to 64 files/directory
    for(id=0; id<64;id++){
        if(directoryBuffer[id*16] == parentIndex){
            count++;
            if(directoryBuffer[id*16+1] == 0xFF){
                // Check if entry index 0xFF, it's a directory. Beside that is a files
                interrupt(0x21, 0x00, "(dir)  ", 0, 0);
            }else{
                interrupt(0x21, 0x00, "(file) ", 0, 0);
            }
            // Output file or directory name
            for(nameLength=0; nameLength<14; nameLength++){
                if(directoryBuffer[id*16 + 2 + nameLength] == '\0'){
                    break;
                }
                interrupt(0x10, 0xE00 + directoryBuffer[id*16 + 2 + nameLength], 0, 0, 0);
            }
            interrupt(0x21, 0x00, "\r\n\0", 0, 0);
        }
    }
    // If there is no files/directory print a string that indicates it
    if (count== 0){
		interrupt(0x21, 0x00, "Direktori ini kosong\r\n\0", 0, 0);
	}
}

void cd(char *fileName){
    // Initialization
    int id, found, len;
    found = -1;
    for(len=0; fileName[len]!='/' && fileName[len]!='\r'; len++){}
    // Mengecek apakah cd . atau cd ..
    // Mencari folder yang namanya sesuai
    for(id=0; id<64; id++){
        if(directoryBuffer[id*16] == curdir){
            if(directoryBuffer[id*16+1] == 0xFF){
                if(stringLength(directoryBuffer + (id*16 + 2), 14) == len){
                    if(strcmpn(directoryBuffer + (id*16 + 2), fileName, len)){
                        found = id;
                    }
                }
            }
        }
    }
    if(len==2 && fileName[0] == '.' && fileName[1] == '.'){
        if(curdir==0xFF){
            interrupt(0x21, 0x00, "Udah di root\r\n\0", 0,0);
        } else{
            while (!(curDirName[itrDirName] == '/')) {
                curDirName[itrDirName--] = '\0';
            }
            curDirName[itrDirName] = '\0';
            curdir = directoryBuffer[16*curdir];
        }        
    }else if(found<0){
        if(!(len == 1 && fileName[0] == '.')){
            interrupt(0x21, 0x00, "Gaada folder dengan nama ini\r\n\0", 0, 0);
        }
    }else{
        //Mengupdate curDirName dan curdir
        curDirName[itrDirName] = '/';
        itrDirName++;
        for(id = 0; id<len; id++){
            curDirName[itrDirName] = fileName[id];
            itrDirName++;
        }
        curdir = found;
    }
    if(fileName[len]=='/'){
        cd(fileName + len + 1);
    }
}

void mkdir(char* folderName){
    //foldername yang dipassing ternyata kelebihan 2 karakter \r\n
    //akan dihapus terlebih dahulu

    int id, found, icpy, len;
    if(stringLength(folderName, 17)==17){
        interrupt(0x21, 0x00, "Nama folder kepanjangan a\r\n\0", 0, 0);
        return;
    }
    len = stringLength(folderName, 16);
    folderName[len-2] = 0;
    folderName[len-1] = 0;
    //Mencari tempat kosong untuk menyimpan folder jika tidak ada yang namanya sama
    //mulai dari 1 karena 0 untuk shell
    for(id=1; id<64; id++){
        if(directoryBuffer[16*id+1]==0xFF){
            if(directoryBuffer[16*id] == curdir){
                if(stringLength(directoryBuffer+16*id+2, 14)==len-2){
                    if(strcmpn(folderName, directoryBuffer+16*id+2, len-2)){
                        break;
                    }
                }
            }
        }
        if(directoryBuffer[16*id+1]==0){
            break;
        }
    }
    if(id==64){
        interrupt(0x21, 0x00, "Not enough space for mkdir gan\r\n\0", 0, 0);
    }else if(directoryBuffer[16*id+1]==0){
        directoryBuffer[16*id]=curdir;
        directoryBuffer[16*id+1]=0xFF;
        for(icpy=0; icpy<len-2; icpy++){
            directoryBuffer[16*id+2+icpy]=folderName[icpy];
        }
        for(icpy=len-2; icpy<14; icpy++){
            directoryBuffer[16*id+2+icpy]=0;
        }
    }else{
        interrupt(0x21, 0x00, "Nama folder sudah terpakai dalam level ini\r\n\0", 0, 0);
    }
}

void ln(char parentIndex, char* fileName1, char* fileName2){
    char fileContent[512*16];
    int i, isSuccess, isSuccess2;
    for (i = 0; i < 512*16; i++) {
		fileContent[i] = 0x0;
	}
    interrupt(0x21, 0x04 + parentIndex*256, fileContent, fileName1, &isSuccess);           
    if (isSuccess==1){
        interrupt(0x21, 0x05 + (parentIndex)*256, fileContent, fileName2, &isSuccess2);
        if(isSuccess2==1){
            interrupt(0x21, 0x00, "Berhasil ln mode hard link\r\n\0", 0, 0);
        }else if(isSuccess2==-3){
            interrupt(0x21, 0x00, "Sektor tidak cukup\r\n\0", 0, 0);
        }else if(isSuccess2==-2){
            interrupt(0x21, 0x00, "Sektor file tidak cukup\r\n\0", 0, 0);
        } else if(isSuccess2==-1){
            interrupt(0x21, 0x00, "File nama sama sudah ada brow\r\n\0", 0, 0);
        }else if(isSuccess2==-4) {
            interrupt(0x21, 0x00, "Sektor file tidak cukup\r\n\0", 0, 0);
        }else{
            interrupt(0x21, 0x00, "Uknown error\r\n\0", 0, 0);
        }
    } else if(isSuccess==-1) {
        interrupt(0x21, 0x00, "File yang akan di-link tidak ditemukan dalam direktori ini\r\n\0", 0, 0);
    }
            
    // Load ulang files sector at 0x101 and 0x102 karena ada perubahan setelah writefile berhasil
    interrupt(0x21, 0x2, directoryBuffer, 0x101, 0);
    interrupt(0x21, 0x2, directoryBuffer+512, 0x102, 0);
};

int stringLength(char *string, int max) {
	int length = 0;
	while (string[length] != 0 && length < max) {
		length++;
	}
	return length;
}

int strcmp(char* strA, char* strB){
    int i = 0;
    int result =1;
    while(i<128 && result){
        if(strA[i] != strB[i]){
            //both string is different
            result = 0;
        } else if(strA[i]=='\0' && strB[i] == '\0'){
            //both string is same and have length <128 
            i = 128; 
        } else{
            i++;
        }
    }
    return result;
}

int strcmpn(char* strA, char* strB, int n){
    // Compare half of string 
    int i = 0;
    int result = 1;
    while(i<n && result){
        if(strA[i] != strB[i]){
            //both string is different
            result = 0;
        } else{
            i++;
        }
    }
    return result;
}