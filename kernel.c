
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

// Milestone 2
void readSector(char *buffer, int sector);
void writeSector(char *buffer, int sector);
void writeFile(char *buffer, char *path, int *sectors, char parentIndex);
void readFile(char *buffer, char *path, int *result, char parentIndex);

// Fungsi Penunjang 1
bool isFlExist(char* dir, int parrentIdx, char* name, bool folder, int* foundIdx);
int foundEmptyDir(char* dir);
void writeDir(char* dir, int dirNum, int parrentIdx, int sectorIdx, char* name);


// Fungsi Penunjang 2
void clear(char* buffer, int length);       //Fungsi untuk mengisi buffer dengan 0
int mod(int dividend, int divisor);         
int div(int numerator, int denominator);    
int strlen(char* buff);
int strcmp(char* a, char* b);
// Fungsi Graphic dan Hiasan
int modeScreen(int mode);
void drawSquare();
void drawSomething();
void asciiART();


int main() {
    char buff[1024];    // Buff untuk menyimpan banyaknya character dari pengguna
    int dump;
    
    // Tampilkan tampilan awal bios dengan graphic
    dump = modeScreen(2);    // Ganti mode menjadi graph mode (Sekalian clear screen)
    drawSomething();
    interrupt(0x15,0x8600,0,100,0); // wait 1000 ms

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
    // writeFile(b1,"sys/bin/cek15",sectors,0xFF);
    // writeSector(b1,0x104);
    // readSector(b2,0x103+32+3*);
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
    char dir[1024];
    int sec = 0x103;
    bool found = false;
    int dirIdx = 0;               // idx dir
    int dirNum = 16*dirIdx;       // alamat dir
    int dirFree = 0;
    int dirNeed = 0;
    int secIdx = 0;               // idx sector
    int secNum = 16*secIdx;       // idx map ke sector (alamat dir)
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
    readSector(dir,0x101);
    readSector(dir+512,0x102);
    
    // 2. Cek dir yang kosong
    while (dirIdx < 64) {     // ada 64 dir yang bisa diisi
        // printString("t1a\n");
        if (dir[dirNum] == 0x00 && dir[dirNum+1] == 0x00 && dir[dirNum+2] == 0x00){    // Kemungkinan ada bug
            dirFree++;
		} 
        dirIdx++;
        dirNum = 16*dirIdx;
        
    }
    
    if (dirFree == 0) {
        printString("Tidak cukup entri di files\n");
        *sectors = -2;
        return;
    }
    // printString("t2\n");
    // 3. Cek parrent folder valid
    found = false;
    if (parentIndex == 0xFF) { // folder di root
        found = true;
    } else if (dir[16*parentIndex+1] == 0xFF) { // sebuah folder
        found = true;
    }

    if (!found) {
        printString("Folder tidak valid\n");
        *sectors = -4;
        return;
    }
    

    // 4a. Atur bagian path
    // Hitung ada berapa dir yang harus dibuat
    i = 0;
    while (path[i] != '\0') {
        if (path[i] == '/') {  // Hitung ada berapa folder
            dirNeed++;
        }
        i++;
    }
    dirNeed++;  // Tambah 1 file

    // Cek apakah jumlah dir tidak cukup
    if (dirNeed > dirFree) {
        printString("Tidak cukup entri di files\n");
        *sectors = -2;
        return;
    }

    // 3b. Buat folder
    i = 0;
    j = 0;
    currParentIdx = parentIndex;
    while (path[i] != '\0') {
        currFlName[j] = path[i];
        if (currFlName[j] == '/') {
            currFlName[j] = '\0';
            // Cek apakah sudah tersedia folder yang sama
            found = isFlExist(dir,currParentIdx,currFlName,true,foundIdx);
            if (found) {
                printString("Folder sudah ada\n");
                currParentIdx = *foundIdx;
                // printString(currParentIdx);
            } else {
                // cari dir yang kosong
                dirIdx = foundEmptyDir(dir);
                dirNum = dirIdx*16;
                // buat folder baru
                writeDir(dir,dirNum,currParentIdx,0xFF,currFlName);

                currParentIdx = dirIdx;
            }
            j = 0;
        } else {
            j++;
        }
        i++;
    }

    // 4. Buat File
    // 4a. Cek apakah sudah file udah ada
    found = isFlExist(dir,currParentIdx,currFlName,false,foundIdx);
    if (found) {
        printString("File sudah ada\n");
        *sectors = -1;
        return;
    }
    // cari dir yang kosong
    dirIdx = foundEmptyDir(dir);
    dirNum = dirIdx*16;


    // 4b. Cek jumlah sektor di map cukup untuk buffer file
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

    // 4c. Buat File
    writeDir(dir,dirNum,currParentIdx,secIdx,currFlName);


    // 5. Cari Sektor di Map yang kosong (sudah ketemu)
    // 6. Tulis Semua Buffer
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
	writeSector(dir,0x101);
    writeSector(dir+512,0x102);
    printString("Write File Success!\n");
    *sectors = secIdx;
    return;
}



// Fungsionalitas Tambahan

bool isFlExist(char* dir, int parrentIdx, char* name, bool folder, int* foundIdx) {
    // return index file yang ditemukan pada foundIdx
    int i,j;
    char buffName[15];
    bool found = false;

    i = 0;
    while (i < 64 && !found) {
        if (dir[16*i] == parrentIdx) {            // kalau ternyata ada yang parent indexnya sama
            for (j = 0; j < 14; j++) {
                buffName[j] = dir[16*i+(2+j)];    
            }
            buffName[14] = '\0';
            if (strcmp(buffName,name)) {         // cek apakah namanya sama
                if ((folder && dir[16*i+1] == 0xFF) || (!folder && dir[16*i+1] != 0xFF)) {  // cek jenisnya sama
                    found = true;
                }   
            }
        }
        i++; 
    }
    *foundIdx = --i;
    return found;
}

int foundEmptyDir(char* dir) {
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


void writeDir(char* dir, int dirNum, int parrentIdx, int sectorIdx, char* name) {
    int i,j;

    dir[dirNum] = parrentIdx;
    dir[dirNum+1] = sectorIdx;
    i = dirNum + 2;
    
    j = 0;
    while (i < dirNum+16 && j < strlen(name)) { 
        dir[i] = name[j];
        i++;
        j++;
    }
    while (i < dirNum+16) {    // Padding yang kosong
        dir[i] = 0x0;
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