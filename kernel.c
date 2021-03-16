// KAMUS
#define INT_10H 0x10
#define INT_13H 0x13
#define INT_16H 0x16

#define bool unsigned char
#define true 1
#define false 0

int nul = 0x0;      // null (\0)
int bs = 0x8;       // backspace (\b)
int ht = 0x9;       // horizontal tab (\t)
int lf = 0xa;       // line feed (\n) 
int vt = 0xb;       // vertical tab (\v)
int ff = 0xc;       // form feed (\f)
int cr = 0xd;       // carriage return (\r) (enter)
int esc = 0x1b;     // escape (\e)

// Graphic mode
int max_X = 320;
int max_Y = 200;
// int mode;


// Fungsi Bawaan
void putInMemory (int segment, int address, char character);
void handleInterrupt21 (int AX, int BX, int CX, int DX);
int interrupt(int number, int AX, int BX, int CX, int DX);

// Milestone 1
void printString(char *string);
void readString(char *string);
void printInteger(int num);
char IntToChar(int num);
// Milestone 2
void readSector(char *buffer, int sector);
void writeSector(char *buffer, int sector);
void writeFile(char *buffer, char *path, int *sectors, char parentIndex);
void readFile(char *buffer, char *path, int *result, char parentIndex);

// Fungsi Penunjang 1
bool isFlExist(char* dir, int parrentIdx, char* name, bool folder, int* foundIdx);
void getFlName(char* files,char filesIdx, char* name);
int foundEmptyDir(char* dir);
void writeDir(char* dir, int dirNum, int parrentIdx, int sectorIdx, char* name);
//bool isFirstLetter(char* first, char* compare);
//int findFileIndex(char parentIndex, char* path);


// Fungsi Penunjang 2
void clear(char* buffer, int length);       //Fungsi untuk mengisi buffer dengan 0
int mod(int dividend, int divisor);         
int div(int numerator, int denominator);    
int strlen(char* buff);
int strcmp(char* a, char* b);
int strcpy(char* dest, char* src);
//int delFirstDigit(int num);
// Fungsi Graphic dan Hiasan
int modeScreen(int mode);
void drawSquare();
void drawSomething();
void asciiART();


int main() {
    char buff[1024];    // Buff untuk menyimpan banyaknya character dari pengguna
    int dump;
    char b1[512];
    char b2[512];
    int* sectors;
    int ascii;
    //char* tes1 = "test1";
    //char* tes2 = "test4";
    //char* tes3;
    // Tampilkan tampilan awal bios dengan graphic
    // dump = modeScreen(2);    // Ganti mode menjadi graph mode (Sekalian clear screen)
    // drawSomething();
    // interrupt(0x15,0x8600,0,30,0); // wait CK:100 = 1000 ms

    // Tampilkan tampilan awal bios dengan ASCII ART
    dump = modeScreen(0);    // Ganti mode menjadi text mode (Sekalian clear screen)
    // handleInterrupt21(0,"\n",0,0);
    // asciiART();
    // handleInterrupt21(0,"\n",0,0);
    // handleInterrupt21(0,"\n",0,0);

    // Say Hello To World!
    handleInterrupt21 (0, "Hello World\n", 0, 0);
    
    // Test writeFile
    // printString("t0\n");
    // b1[0] = 'a';
    // b1[1] = 'b';
    // b1[2] = 'c';
    // b1[3] = 'd';
    // writeFile(b1,"i1/i2/../i3",sectors,0xFF);
    // readFile(b2,"/sys/cek3",sectors,0xFF);
    //printInt(12);
    //tes3 = append(tes1,tes2);
    //printString(tes3);
    /*
    printString("1");
    printString("2");
    printString("3");
    printString("4");
    printString("5\n");
    printInteger(12345);
    printString("\n");
    */

    //printInteger(12345);
    //writeFile("KLMNO", "coba", sectors,0xFF);
    //writeFile("test123123","sys/test/t1",sectors,0xFF);
    //readFile(b2,"sys/test/t1",sectors,0xFF);
    //printString(b2);
    shell();
    // printString(b2);

    while (1) {
        // Loop selamanya untuk meminta input string dari user dan menampilkannya pada layar
        
        readString(buff);
        //if(strcmp(cmd,"c")){
        //    printString("calling cd\n");
        //}
        //launchProgram()
        printString(buff);
        printString("\n");
    };
}


// Mode yang tersedia dan dapat digunakan
int modeScreen(int mode) {
    /* mode 0 : default text
       mode 1 : text ART
       mode 2 : graphical
    */
    switch (mode)
    {
    case 1:
        interrupt(0x10,0x0000,0,0,0);
        break;
    case 2:
        interrupt(0x10,0x0013,0,0,0);
        break;
    default:
        interrupt(0x10,0x0003,0,0,0);
        break;
    }
    return;
}

// Membuat Interupt21 (DOS Interupt) untuk menjalankan perintah dari pengguna
void handleInterrupt21 (int AX, int BX, int CX, int DX){
    switch (AX) {
        case 0x0:
            printString(BX);
            break;
        case 0x1:
            readString(BX);
            break;
        default:
            printString("Invalid interrupt");
    }
}

// Menampilkan string/tulisan pada layar
void printString(char *string) {
    int AL = 0x0E00;
    int i = 0;
    while (string[i] != '\0') {
        interrupt(INT_10H, AL + string[i],0x000F,0,0);

        if (string[i] == '\n') {
            // interrupt(INT_10H,AL+lf,0,0,0);
            interrupt(INT_10H,AL+cr,0x000F,0,0);
        }
        i++;
    }
    
    
}

// Membaca input dari pengguna
void readString(char* string) {    
    int AL = 0x0E00;
    int loop = 1;
    int i = 0;
    int ascii;

    while (loop)
    {
        ascii = interrupt(INT_16H,0,0,0,0);
        if (ascii == cr) {
            string[i] = nul;
            interrupt(INT_10H,AL+lf,0,0,0);
            interrupt(INT_10H,AL+cr,0,0,0);
            loop = 0;
        } else if (ascii == bs) {
            if (i > 0 && mod(i,80) != 0) {
                interrupt(INT_10H,AL+bs,0,0,0);
                interrupt(INT_10H,AL+nul,0,0,0);
                interrupt(INT_10H,AL+bs,0,0,0);
                i--;
            }
        } else {
            string[i] = ascii;
            interrupt(INT_10H,AL+ascii,0,0,0);
            i++;
        }      
    }   
}

void printInteger(int num){
    char Result[20];
    int i;
    int temp;
    char tempchar;
    i  = 0;
    //clear(Result,20);
    while(num >= 1){
        temp = mod(num,10);
        Result[i] = IntToChar(temp);
        //printString(Result[i]);
        i++;
        num = div(num,10);
    }
    i--;
    while(i > 0){
        tempchar = Result[i];
        printString(tempchar);
        i--;
    }
}
char IntToChar(int num){
    switch (num)
    {
    case 0:
        printString("0");
        return "0";
        break;
    case 1:
        printString("1");
        return "1";
        break;
    case 2:
    printString("2");
        return "2";
        break;
    case 3:
    printString("3");
        return "3";
        break;
    case 4:
    printString("4");
        return "4";
        break;
    case 5:
    printString("5");
        return "5";
        break;
    case 6:
    printString("6");
        return "6";
        break;
    case 7:
    printString("7");
        return "7";
        break;
    case 8:
    printString("8");
        return "8";
        break;
    case 9:
    printString("9");
        return "9";
        break;
    
    default:
    printString("X");
        return "X";
        break;
    }
}

void readSector(char *buffer, int sector) {
    int AL = 0x0200;
    int num = 0x01;
    int cs = div(sector, 36) * 0x100 + mod(sector, 18) + 1;
    int hd = mod(div(sector, 18), 2) * 0x100;
    interrupt(INT_13H, AL+num, buffer, cs, hd);
}

void writeSector(char *buffer, int sector) {
    int AL = 0x0300;
    int num = 0x01;
    int cs = div(sector, 36) * 0x100 + mod(sector, 18) + 1;
    int hd = mod(div(sector, 18), 2) * 0x100;
    interrupt(INT_13H, AL+num, buffer, cs, hd);
}

void writeFile(char *buffer, char *path, int *sectors, char parentIndex) {
    // path jadi nama
    // KAMUS
    
    char map[512];
    char files[1024];
    int sec = 0x103;
    bool found = false;
    int filesIdx = 0;               // idx files
    int filesNum = 16*filesIdx;       // alamat files
    int secIdx = 0;               // idx sector
    int secNum = 16*secIdx;       // idx map ke sector (alamat files)
    int countSec = 0;
    int i, j, k, l;
    char buffSec[512];
    char buffName[15];
    char currFlName[128];
    char currParentIdx;
    int *foundIdx;
    
    // printString("t1\n");
    // 1. Baca Sektor Map dan Dir
    readSector(map,0x100);
    readSector(files,0x101);
    readSector(files+512,0x102);
     
    // printString("t2\n");
    // 2. Cek parrent folder valid
    found = false;
    if (parentIndex == 0xFF) { // folder di root
        found = true;
    } else if (files[16*parentIndex+1] == 0xFF) { // sebuah folder
        found = true;
    }

    if (!found) {
        printString("Folder tidak valid\n");
        *sectors = -4;
        return;
    }
    

    // 3. Atur bagian path
    // 3a. Buat folder
    i = 0;
    j = 0;
    if (path[i] == '/') {   // dari root
        currParentIdx = 0xFF;
        i++;  
    } else {
        currParentIdx = parentIndex;
    }

    
    while (path[i] != '\0') {
        currFlName[j] = path[i];
        
        if (currFlName[j] == '/') {
            
            currFlName[j] = '\0';

            if (strcmp(currFlName,".")) 
            {
                // Do Nothing, di curr dir yang sama
            } 
            else if (strcmp(currFlName,"..")) 
            {   // Back to parent
                if (currParentIdx != 0xFF) {    // Bukan di root
                    // Cari parent folder ini
                    currParentIdx = files[(currParentIdx)*16];
                }
                // kalau di root do nothing aja
            } 
            else if (strcmp(currFlName,"")) 
            {
                printString("Error: Nama folder tidak valid\n");
                 *sectors = -5;
                return;
            } else {
                // Cek apakah sudah tersedia folder yang sama
                found = isFlExist(files,currParentIdx,currFlName,true,foundIdx);
                if (found) {
                    printString("Folder sudah ada\n");
                    currParentIdx = *foundIdx;
                    // printString(currParentIdx);
                } else {
                    // cari files yang kosong
                    filesIdx = foundEmptyDir(files);
                    if (filesIdx == -1) {
                        printString("Tidak cukup entri di files\n");
                        *sectors = -2;
                        return;
                    }

                    filesNum = filesIdx*16;
                    // buat folder baru
                    writeDir(files,filesNum,currParentIdx,0xFF,currFlName);

                    currParentIdx = filesIdx;
                }
            }
            

            j = 0;  
        } else {
            j++;
        }
        i++;
        
    }
    currFlName[j] = path[i];
    // Cek apakah nama file valid (tidak boleh kosong, kalau kosong berarti cuma create folder doang)
    
    
    if (strcmp(currFlName,"")) {
        printString("Nama file tidak valid");
        *sectors = -5;
        return;
    }

    // 3b. Cek apakah sudah file udah ada
    found = isFlExist(files,currParentIdx,currFlName,false,foundIdx);
    if (found) {
        printString("File sudah ada\n");
        *sectors = -1;
        return;
    }
    // cari files yang kosong
    filesIdx = foundEmptyDir(files);
    if (filesIdx == -1) {
        printString("Tidak cukup entri di files\n");
        *sectors = -2;
        return;
    }
    filesNum = filesIdx*16;


    // 4. Cek jumlah sektor di map cukup untuk buffer file
    // 1 File mengisi 16 sector (asumsi jadiin chunk gini)
    // Terdapat 32 secIdx yang bisa diisi (chunk), dengan 1 chunk berisi 16 sektor
    // 1 sector memuat 512 byte
    found = false;
    while(secIdx < 32 && !found) {
        if (map[secNum] == 0x00) {
            found = true;
        } else {
            secIdx++;
            secNum = 16*secIdx;
        }
    }
    if (!found) {
        printString("Tidak cukup sektor kosong\n");
        sectors = -3;
        return;
    }

    // 5. Buat File
    writeDir(files,filesNum,currParentIdx,secIdx,currFlName);


    // 6. Cari Sektor di Map yang kosong (sudah ketemu)
    // 7. Tulis Semua Buffer
    // secNum = 32+secIdx*16    // 32 sector kernel, secIdx*16 karena setiap file ada 16 sector + i
    // Implementasi panjang buffer
    countSec = div(strlen(buffer)-1,512) + 1;
    for (i = 0; i < countSec; i++) {
        map[secNum+i] = 0xFF;
        for (j = 0; j < 512; j++) {
            buffSec[j] = buffer[j+512*i];
        } 
        writeSector(buffSec,sec+secNum+i); 
    }
    

    writeSector(map,0x100);
	writeSector(files,0x101);
    writeSector(files+512,0x102);
    printString("Write File Success!\n");
    *sectors = secIdx;
    return;
}


void readFile(char *buffer, char *path, int *result, char parentIndex) {
    char files[1024], maps[512];
    int sectNum, sectPos, sectIdx, fileIdx;
    char currParentIdx;
    char currFlName[128];
    bool found;
    int* foundIdx;
    int i,j;

    // 0. inisialisasi
    readSector(maps, 0x100);
    readSector(files, 0x101);
    readSector(files + 512, 0x102);

    // 1. cari folder dan file
    i = 0;
    j = 0;
    if (path[i] == '/') {   // dari root
        currParentIdx = 0xFF;
        i++;  
    } else {
        currParentIdx = parentIndex;
    }

    // 2. Cari path sampe ketemu bagian file
    while (path[i] != '\0') {
        currFlName[j] = path[i];
        
        if (currFlName[j] == '/') {
            
            currFlName[j] = '\0';

            if (strcmp(currFlName,".")) 
            {
                // Do Nothing, di curr dir yang sama
            } 
            else if (strcmp(currFlName,"..")) 
            {   // Back to parent
                if (currParentIdx != 0xFF) {    // Bukan di root
                    // Cari parent folder ini
                    currParentIdx = files[(currParentIdx)*16];
                }
                // kalau di root do nothing aja
            } 
            else if (strcmp(currFlName,"")) 
            {
                printString("Error: Nama folder tidak valid\n");
                 *result = -5;
                return;
            } else {
                // Cek apakah sudah tersedia folder yang sama
                found = isFlExist(files,currParentIdx,currFlName,true,foundIdx);
                if (found) {
                    printString("Folder sudah ada\n");
                    currParentIdx = *foundIdx;
                    // printString(currParentIdx);
                } else {
                    printString("Folder tidak ditemukan\n");
                    *result = -1;
                    return;
                }
            }
            j = 0;  
        } else {
            j++;
        }
        i++;
        
    }
    currFlName[j] = path[i];

    // while (path[i] != '\0') {
    //     currFlName[j] = path[i];
    //     if (currFlName[j] == '/') {
    //         currFlName[j] = '\0';
    //         // Cek apakah nama folder valid (tidak boleh kosong)
    //         if (strcmp(currFlName,"")) {
    //             printString("Nama folder tidak valid");
    //             *result = -5;
    //             return;
    //         }
             
    //         // Cek apakah sudah tersedia folder yang sama
    //         found = isFlExist(files,currParentIdx,currFlName,true,foundIdx);
    //         if (found) {
    //             printString("Folder sudah ada\n");
    //             currParentIdx = *foundIdx;
    //             // printString(currParentIdx);
    //         } else {
    //             printString("Folder tidak ditemukan\n");
    //             *result = -1;
    //             return;
    //         }
    //         j = 0;
    //     } else {
    //         j++;
    //     }
    //     i++;
    // }

    // Cek apakah nama file valid (tidak boleh kosong)
    if (strcmp(currFlName,"")) {
        printString("Nama file tidak valid");
        *result = -5;
        return;
    }

    // 3. Cek file ada
    found = isFlExist(files,currParentIdx,currFlName,false,foundIdx);
    if (!found) {
        printString("File tidak ditemukan\n");
        return;
    }

    printString("File ditemukan\n");
    fileIdx = *foundIdx;

    // Read sector index
    sectIdx = files[fileIdx * 16 + 1];

    // Read file
    // Ada 16 sektor
    for (sectNum = 0; sectNum < 16; sectNum++) {
        sectPos = sectIdx * 16 + sectNum;
        
        if (maps[sectPos] == 0) { // EOF
            break;
        }
        readSector(buffer, 0x103 + sectPos);
    }

    *result = 1;
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



// Bonus - ASCII ART
void asciiART() {
    printString("  :'#######:::'######::'##:::'##::::'###::::'########:::'#######::'##::: ##:\n");
    printString("  '##.... ##:'##... ##: ##::'##::::'## ##::: ##.... ##:'##.... ##: ###:: ##:\n");
    printString("   ##:::: ##: ##:::..:: ##:'##::::'##:. ##:: ##:::: ##: ##:::: ##: ####: ##:\n");
    printString("   ##:::: ##:. ######:: #####::::'##:::. ##: ##:::: ##: ##:::: ##: ## ## ##:\n");
    printString("   ##:::: ##::..... ##: ##. ##::: #########: ##:::: ##: ##:::: ##: ##. ####:\n");
    printString("   ##:::: ##:'##::: ##: ##:. ##:: ##.... ##: ##:::: ##: ##:::: ##: ##:. ###:\n");
    printString("   ##:::: ##:'##::: ##: ##:. ##:: ##.... ##: ##:::: ##: ##:::: ##: ##:. ###:\n");
    printString("  . #######::. ######:: ##::. ##: ##:::: ##: ########::. #######:: ##::. ##:\n");
    printString("  :.......::::......:::..::::..::..:::::..::........::::.......:::..::::..::\n\n");
    printString("                                 Created By:\n");
    printString("                             - Ronggur (13519008)\n");
    printString("                             - Azhar   (13519020)\n");
    printString("                             - Alge    (13519039)\n");
    
}


// Menggambar kotak pada mode 13h
void drawSquare() {
    int x = 0;
    int y = 0;
    int AL = 0x0C00;
    
    while (y < max_Y) {
        while (x < max_X){
            if ((x >= 100 && x <= 150) && (y >= 50 && y <= 100)) {
                interrupt(INT_10H,AL + 14,0,x,y);
            }
            x++;
        }
        x=0;
        y++;
    }   
}


// Clear Screen
void clear(char* buffer, int length){
    int i = 0;
    while (i < length) {
        buffer[i] = 0x0;
    }
    
}

// Modulo
int mod(int dividend, int divisor){
    while (dividend >= divisor){
        dividend -= divisor;
    }
    return dividend;
}

// Division
int div(int numerator, int denominator){
    int res = 0;
    while (numerator >= denominator * res){
        res += 1;
    }
    res -= 1;
    return res;
}

int strlen(char* buff) {
    int i = 0;
    while (buff[i] != '\0') {
        i++;
    }
    return i;
    
}
int strcmp(char* a, char* b){
    int lenA;
    int i;
    
    lenA = strlen(a);
    if(lenA != strlen(b)){
        return 0;
    }
    i = 0;
    while(i < lenA){
        if(a[i] != b[i]){
            return 0;
        }
        i++;
    }
    return 1;
}
int strcpy(char* dest, char* src){
    int i = 0;
    clear(dest,strlen(dest));
    for(i;i<strlen(src);i++){
        dest[i] = src[i];
    }
}
// Menggambar image di mode13, error jangan digunakan
void drawSomething() {
    char image[512];
    int sec = 1024;
    int address;
    int x_size;
    int y_size;
    int halfDif_x;
    int halfDif_y;
    int x,y,i,nSec;

    
    nSec = 0;
    readSector(image,sec + nSec);
    
    
    x_size = image[0];
    y_size = image[1];

    // printString("x: ");
    // printString(x_size);
    // printString("\ny: ");
    // printString(y_size);

    
    // Coba bikin ketengah
    halfDif_x = div((max_X - x_size),2);
    halfDif_y = div((max_Y - y_size),2);

    x = halfDif_x;
    y = halfDif_y;
    
    i = 2;

    while (y < y_size + halfDif_y) {
        while (x < x_size + halfDif_x) {
            if (i >= 512) {
                nSec++;
                readSector(image,sec + nSec);
                i = 0; 
                // printString("a");
            }
            address = 320*y + x;
            putInMemory(0xA000,address,image[i]);
            i++;
            x++;
        }
        x = halfDif_x;
        y++;
    }
    // // return;
}




/*** Unused
int findFileIndex(char* dir, char *path, int *result, char parentIndex) {
    // cari file ada di index keberapa di dir
    char files[1024], parent;
    int fileIdx, pathIdx;
    bool isFound;

    parent = parentIndex;
    fileIdx = 0;
    pathIdx = 0;
    isFound = false;

    readSector(files, 0x101);
    readSector(files + 512, 0x102);

    if (path[0] == "/") { // root
        pathIdx += 1;
        parent = 0xFF;
    }
    else if (path[0] == "." && path[1] == "/") { // current dir
        pathIdx += 2;
    }

    while (path[pathIdx] != 0) {
        if (path[pathIdx] == "/") { // enter dir
            if (!isFound) {
                return -1;
            }
            isFound = false;
            pathIdx += 1;
        }
        else if (path[pathIdx] == "." && path[pathIdx + 1] == "." && path[pathIdx + 2] == "/") {
            if (parent == 0xFF) {
                return -1;
            }
            parent = files[parent * 16];
            pathIdx += 3;
        }
        else { // traverse semua file di dir
            if (parent != files[fileIdx * 16]) { // file gak di parent
                fileIdx += 1;
            }
            else if (files[fileIdx * 16 + 2] == 0) { // nama kosong
                fileIdx += 1;
            }
            // file found
            // kalo huruf pertama path sama file sama, pathIdx diupdate
            else if (isFirstLetter(path + pathIdx, files + fileIdx * 16 + 2)) {
                pathIdx += strlen(files + fileIdx * 16 + 2);
                parent = fileIdx;
                fileIdx = 0;
                isFound = true;
            }
            else {
                fileIdx += 1;
            }

            if (fileIdx >= 64) { // max 64 files
                return -1;
            }
        }
    }

    return parent;
}


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