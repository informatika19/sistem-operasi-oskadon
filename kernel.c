void handleInterrupt21 (int AX, int BX, int CX, int DX);
void printString(char *string);
//void readString(char *string);
//void clear(char *buffer, int length); //Fungsi untuk mengisi buffer dengan 0
//int length(char *string);
static CurLine = 0xa0*0x10;

int main () {
  printHelloWorld();
  printHelloWorld();
  //printString("wkwk");
  //printString("wkwk");
  //makeInterrupt21();
  while (1);

//  ;
//  printHelloWorld();
//  while (1);

}
void handleInterrupt21 (int AX, int BX, int CX, int DX) {}
/*
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
*/
int get_curr_line(){
    CurLine =  CurLine + 0xa0;
    return CurLine;
}
int length(char *string){
    return sizeof(&string)/ sizeof(&string[0]) ;
}
printHelloWorld(){
  int line = get_curr_line();
  putInMemory(0xB000, 0x8000 + line, 'h');
  putInMemory(0xB000, 0x8001+ line, 0xD);
  putInMemory(0xB000, 0x8002+ line, 'e');
  putInMemory(0xB000, 0x8003+ line, 0xD);
  putInMemory(0xB000, 0x8004+ line, 'l');
  putInMemory(0xB000, 0x8005+ line, 0xD);
  putInMemory(0xB000, 0x8006+ line, 'l');
  putInMemory(0xB000, 0x8007+ line, 0xD);
  putInMemory(0xB000, 0x8008+ line, 'o');
  putInMemory(0xB000, 0x8009+ line, 0xD);
  putInMemory(0xB000, 0x800a+ line, ' ');
  putInMemory(0xB000, 0x800b+ line, 0xD);
  putInMemory(0xB000, 0x800c+ line, 'w');
  putInMemory(0xB000, 0x800d+ line, 0xD);
  putInMemory(0xB000, 0x800e + line, 'o');
  putInMemory(0xB000, 0x800f+ line, 0xD);
  putInMemory(0xB000, 0x8010+ line, 'r');
  putInMemory(0xB000, 0x8011+ line, 0xD);
  putInMemory(0xB000, 0x8012+ line, 'l');
  putInMemory(0xB000, 0x8013+ line, 0xD);
  putInMemory(0xB000, 0x8014+ line, 'd');
  putInMemory(0xB000, 0x8015+ line, 0xD);
}
/* // salah
void printString(char *string){
    int line = get_curr_line();
    int i = 0;
    while(string[i] != "\0"){
        putInMemory(0xB000, 0x8000 + 2*i + line, &string[i]);
        putInMemory(0xB000, 0x8000 + 2*i + 1 + line, 0xD);
        i++;
    }
}
void printString(char *string){
    int line = get_curr_line();
    int i = 0;
    for (i; i < length(*string);i++){
        putInMemory(0xB000, 0x8000 + 2*i + line, &string[i]);
        putInMemory(0xB000, 0x8000 + 2*i + 1 + line, 0xD);
    }
}
*/
