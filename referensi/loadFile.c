// loadFile.c
// loadFile baru dpt dilakukan ke root

#include <stdio.h>

void main(int argc, char* argv[]) {
  int i;

  // argc only blank or space
  if (argc < 2) {
    printf("Specify file name to load\n");
    return;
  }

  // open the source file
  FILE* loadFil;
  loadFil = fopen(argv[1], "r");
  if (loadFil == 0) {
    printf("File not found\n");
    return;
  }

  // open the floppy image (system.img)
  FILE* floppy;
  floppy = fopen("system.img", "r+");
  if (floppy == 0) {
    printf("system.img not found\n");
    return;
  }

  // load the map.img in 0x100
  char map[512];
  fseek(floppy, 512 * 0x100, SEEK_SET);
  for (i = 0; i < 512; i++) map[i] = fgetc(floppy);

  // load the files img in 0x100 and 0x102
  char files[1024];
  fseek(floppy, 512 * 0x101, SEEK_SET);
  for (i = 0; i < 1024; i++) files[i] = fgetc(floppy);

  // load the sector img in 0x104
  char sector[512];
  fseek(floppy, 512 * 0x103, SEEK_SET);
  for (i = 0; i < 512; i++) sector[i] = fgetc(floppy);

  // find a free entry in the files img
  for (i = 0; i < 1024; i = i + 16)
    if (files[i] == 0 && files[i+1] == 0 && files[i+2] == 0) break;
  if (i == 1024) {
    printf("Not enough room in filesectory\n");
    return;
  }
  int filesindex = i;

  // fill the name field with 00s first (initialization array of char for files name)
  for (i = 0; i < 14; i++) files[filesindex + i + 2] = 0x00;
  // copy the name from argc to files
  for (i = 0; i < 14; i++) {
    if (argv[1][i] == 0) break;
    files[filesindex + i + 2] = argv[1][i];
  }

  // find free entry for sector img
  for (i = 0; i < 32; i++) {
    if (sector[i*16] == 0) break;
  }
  // No sector available 
  if (i == 32) {
    printf("No sector available\n");
    return;
  }

  int sectIndex = i;
  // Set parent index and entry index
  files[filesindex] = 0xFF;
  files[filesindex + 1] = sectIndex;

  // find free sectors and add them to the file
  int sectcount = 0;
  while (!feof(loadFil)) {
    if (sectcount == 20) {
      printf("Not enough space in filesectory entry for file\n");
      return;
    }

    // find a free map entry
    for (i = 0; i < 256; i++)
      if (map[i] == 0) break;
    if (i == 256) {
      printf("Not enough room for file\n");
      return;
    }

    // mark the map entry as taken
    map[i] = 0xFF;


    sector[sectIndex * 16 + sectcount] = i;
    sectcount++;

    printf("Loaded %s to sector %d\n", argv[1], i);

    // move to the sector and write to it
    fseek(floppy, i * 512, SEEK_SET);
    for (i = 0; i < 512; i++) {
      if (feof(loadFil)) {
        fputc(0x0, floppy);
        break;
      } else {
        char c = fgetc(loadFil);
        fputc(c, floppy);
      }
    }
  }

  // write the map and files back to the floppy (system image)
  fseek(floppy, 512 * 0x100, SEEK_SET);
  for (i = 0; i < 512; i++) fputc(map[i], floppy);

  fseek(floppy, 512 * 0x101, SEEK_SET);
  for (i = 0; i < 1024; i++) fputc(files[i], floppy);

  fseek(floppy, 512 * 0x103, SEEK_SET);
  for (i = 0; i < 512; i++) fputc(sector[i], floppy);

  fclose(floppy);
  fclose(loadFil);
}
