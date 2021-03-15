
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
void printInt(int num);

// Milestone 2
void readSector(char *buffer, int sector);
void writeSector(char *buffer, int sector);
int writeFile(char *buffer, char *path, int *sectors, char parentIndex);
void readFile(char *buffer, char *path, int *result, char parentIndex);

// Fungsi Penunjang
void clear(char* buffer, int length);       //Fungsi untuk mengisi buffer dengan 0
int mod(int dividend, int divisor);         
int div(int numerator, int denominator);    
int strlen(char* buff);
int strcmp(char* a, char* b);
void strcpy(char* dest, char* src);
int getFirstDigit(int num);
// Fungsi Graphic dan Hiasan
int modeScreen(int mode);
void drawSquare();
void drawSomething();
void asciiART();
//buatdebugging dulu sementara belum ada loadfile
void shell();
void executecmd(char* cmd);
int modifiedstrcmp(char* a, char* b, int len,int start);
int ignoreSpace(char* cmd, int start);
int getParameterLength(char* cmd, int cmdIndex);
int main() {
    char buff[1024];    // Buff untuk menyimpan banyaknya character dari pengguna
    int dump = interrupt(0x10,0x0003,0,0,0);    // Ganti mode menjadi text mode (Sekalian clear screen)
    char b1[512];
    char b2[512];
    int* sectors;
    char* cmd;
    *sectors = 1;
    

    b1[0] = 0x54;
    b1[1] = 0x55;
    b1[2] = 0x00;

    // Tampilkan tampilan awal bios dengan ASCII ART
    handleInterrupt21(0,"\n",0,0);
    asciiART();
    handleInterrupt21(0,"\n",0,0);
    handleInterrupt21(0,"\n",0,0);

    // Say Hello To World!
    handleInterrupt21 (0, "Hello World\n", 0, 0);
    // writeFile(b1,"cek4",sectors,0xF4);
    // writeSector(b1,0x104);
    readSector(b2,0x100);
    printString(b2);
    //printInt(123456);
    shell();
    while (1) {
        // Loop selamanya untuk meminta input string dari user dan menampilkannya pada layar
        
        readString(cmd);
        //if(strcmp(cmd,"c")){
        //    printString("calling cd\n");
        //}
        //launchProgram()
        printString(cmd);
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
int getFirstDigit(int num){
    while(num >= 10){
        num = div(num,10);
    }
    return num;
}
void printDigit(int num){
    num = mod(num,10);
    switch (num)
    {
    case 0:
        printString("0");
        break;
    case 1:
        printString("1");
        break;
    case 2:
        printString("2");
        break;
    case 3:
        printString("3");
        break;
    case 4:
        printString("4");
        break;
    case 5:
        printString("5");
        break;
    case 6:
        printString("6");
        break;
    case 7:
        printString("7");
        break;
    case 8:
        printString("8");
        break;
    case 9:
        printString("9");
        break;
    default:
        break;
    }
}

void printInt(int num){
    
    int tempDigit;
    printString("Calling PrintInt\n");
    while(num >= 10){
        tempDigit = getFirstDigit(num);
        printDigit(tempDigit);
        num = div(num,10);
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

int writeFile(char *buffer, char *path, int *sectors, char parentIndex) {
    // int* sectors dimanfaatin buat jadi jumlah sector
    // KAMUS
    char map[512];
    char dir[1024];
    int sec = 0x103;
    bool found = false;
    int dirIdx = 0;               // idx dir
    int dirNum = 16*dirIdx;       // alamat dir
    int secIdx = 0;               // idx sector
    int secNum = 32+secIdx*16;    // idx map ke sector (alamat dir)
    int i, j, k;
    char partBuff[512];
    
    
    // 1. Baca Sektor Map dan Dir
    readSector(map,0x100);
    readSector(dir,0x101);
    readSector(dir+512,0x102);
    
    // 2. Cek dir yang kosong
    while (dirIdx < 32 && !found) {     // ada 32 dir yang bisa diisi
        if (dir[dirNum] == 0x00 && dir[dirNum+1] == 0x00 && dir[dirNum+2] == 0x00){    // Kemungkinan ada bug
			found = true;
		} else {
            dirIdx++;
            dirNum = 16*dirIdx;
            // printString("i\n");
        }
    }

    if (!found) {
        printString("Tidak cukup entri di files");
        return -2;
    }

    //  3. Cek jumlah sektor di map cukup untuk buffer file
    // Note, 32 sector sudah dipakai untuk kernel
    // 1 File mengisi 16 sector (asumsi jadiin chunk gini)
    // 1 sector memuat 512 byte, buff panjangnya kelipatan 512;
    found = false;
    
    while(secIdx < 16 && !found) {
        if (map[secNum] == 0x00) {
            found = true;
        } else {
            secIdx++;
            secNum = 32+secIdx*16;
        }
    }

    if (!found) {
        printString("Tidak cukup sektor kosong");
        return -3;
    }

    // 4. Bersihkan sektor (dir) yang akan digunakan untuk menyimpan nama file
    dir[dirNum] = parentIndex;
    dir[dirNum+1] = secIdx;
    i = dirNum + 2;
    // 5. Isi Sektor pada Dir dengan nama file
    // untuk sekarang asumsi jadi nama file dulu si path itu
    j = 0;
    while (i < dirNum+16 && j < strlen(path)) { 
        dir[i] = path[j];
        i++;
        j++;
    }
    while (i < dirNum+16) {    // Padding yang kosong
        dir[i] = 0x0;
        i++;
    }

    // 6. Cari Sektor di Map yang kosong (sudah ketemu)
    // 7. Tulis Semua Buffer
    // secNum = 32+secIdx*16    // 32 sector kernel, secIdx*16 karena setiap file ada 16 sector + i
    for (i = 0; i < *sectors; i++) {
        map[secNum+i] = 0xFF;
        for (j = 0; j < 512; j++) {
            partBuff[j] = buffer[j+512*i];
        } 
        writeSector(partBuff,sec+secNum+i);
        
    }

    writeSector(map,0x100);
	writeSector(dir,0x101);
    writeSector(dir+512,0x102);
    printString("Berhasil!");
    return 0;
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
    printString("calling strcmp");
    printString(a);
    printString(b);
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

void strcpy(char* dest, char* src){
    int i = 0;
    int lenSrc;
    clear(dest,strlen(dest));
    lenSrc =  strlen(src);
    for(i;i<lenSrc;i++){
        dest[i] = src[i];
    }

}
// Menggambar image di mode13, error jangan digunakan
// void drawSomething() {
//     extern char imageFile;
//     char* image = &imageFile;
//     int address;

//     int x_size = image[0];
//     int y_size = image[1];

    
//     // Coba bikin ketengah
//     int halfDif_x = div((max_X - x_size),2);
//     int halfDif_y = div((max_Y - y_size),2);

//     int x = halfDif_x;
//     int y = halfDif_y;
    
//     int i = 2;

//     while (y < y_size + halfDif_y) {
//         while (x < x_size + halfDif_x) {
//             address = 320*y + x;
//             putInMemory(0xA000,address,image[i]);
//             i++;
//             x++;
//         }
//         x = halfDif_x;
//         y++;
//     }
//     // return;
// }

void shell(){
    char cmd[180];
    int test;
    printString("Executing Shell...\n");
    while(1){
        printString("$ ");
        //test = interrupt(INT_16H,0,0,0,0);
        readString(cmd);
        executecmd(cmd);
    }

}

void executecmd(char* cmd){
    int cmdIndex = 0;
    int paramlength = 0;
    while(1){ // implementasi jangan lupa ilangin loop dan breaknya
        if(modifiedstrcmp(cmd,"cd",2,cmdIndex)){
            cmdIndex += 2;
            cmdIndex = ignoreSpace(cmd,cmdIndex);
            paramlength = getParameterLength(cmd,cmdIndex);
            printString("calling cd\n");
            printInt(paramlength);
        }else if(modifiedstrcmp(cmd,"ls",2,cmdIndex)){
            cmdIndex += 2;
            cmdIndex = ignoreSpace(cmd,cmdIndex);
            printString("calling ls\n");
        }else if(modifiedstrcmp(cmd,"cat",3,cmdIndex)){
            cmdIndex += 3;
            cmdIndex = ignoreSpace(cmd,cmdIndex);
            printString("calling cat\n");
        }else if(cmd[cmdIndex] == 0x0 || cmd[cmdIndex] == "\n"){
            cmdIndex = ignoreSpace(cmd,cmdIndex);
            printString("end of command\n");
            break;
        }else{
            cmdIndex = ignoreSpace(cmd,cmdIndex);
            printString("invalid command\n");
            break;
        }
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

int ignoreSpace(char* cmd, int start) { //return new index
    while(cmd[start] == 0x20){
        start++;
    }
    return start;
}
int getParameterLength(char* cmd, int cmdIndex){
    int len = 0;
    printString("calling getParameterLength\n");
    while(cmd[cmdIndex] != 0x20){
        len++;
        cmdIndex++;
    }
    return len;
}