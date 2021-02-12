int main () {
    putInMemory(0xB000, 0x80A0, 'H');
    putInMemory(0xB000, 0x80A1, 0xD);
    putInMemory(0xB000, 0x80A2, 'a');
    putInMemory(0xB000, 0x80A3, 0xD);
    putInMemory(0xB000, 0x80A4, 'i');
    putInMemory(0xB000, 0x80A5, 0xD);
    while (1);
}
void handleInterrupt21 (int AX, int BX, int CX, int DX) {}