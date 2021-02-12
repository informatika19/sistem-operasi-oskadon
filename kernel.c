static CurLine = 0xa0*0x10;

int main () {
  printHelloWorld();
  printHelloWorld();
  while (1);
}
int get_curr_line(){
    CurLine =  CurLine + 0xa0;
    return CurLine;
}
printHelloWorld(){
  int line = get_curr_line();
  putInMemory(0xB000, 0x8000 + line, 'H');
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



void handleInterrupt21 (int AX, int BX, int CX, int DX) {}
