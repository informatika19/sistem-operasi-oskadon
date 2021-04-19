#include "text.h"


// void printString(char *string){
//   int i = 0;
//   char c = string[i];
//   while (c != '\0'){
//     // AH = 0x0E for teletype output , AL = character to print
//     int AL = c;
//     int AH = 0x0E*256 ;
//     int AX = AH + AL;
//     interrupt(0x10, AX, 0, 0, 0);
//     i = i + 1;
//     c = string[i];
//   }
// }
void printString(char* string) {
    int AL = 0x0E00;
    int i = 0;
    while (string[i] != '\0') {
        interrupt(0x10, AL+string[i], 0x000F, 0, 0);

        if (string[i] == '\n') {
            interrupt(0x10, AL+0xd, 0x000F, 0, 0);
        }

        i++;
    }
}

void readString(char *string){
  // initialize variable
  int i = 0; //string's length
  int j = 0; //input position
  int k = 0; //for loop index
  char c = '\0';
  while(1){
    // Wait for keypress and read character from keypress/keyboard
    c = interrupt(0x16, 0, 0, 0, 0);
    // Display input from keyboard
    if(c==0xD){
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
      // Periksa bahwa inputnya shift+,
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

void clear(char *buffer, int length){
  // Clearing buffer
  int i;
	for (i = 0; i < length; i++) {
		buffer[i] = 0;
	}
}

int stringLengthnoMax(char *string) {
    int len = 0;
    char* p = string;
    while(*p) {
      p++; 
      len++;
    }
    return len;

}
int stringLength(char *string, int max) {
	int length = 0;
	while (string[length] != 0 && length < max) {
		length++;
	}
	return length;
}
int stringEqual(char *s1, char *s2, int length){
  int equal = (stringLength(s1, length) == stringLength(s2, length));
  int i;
  for(i=0; i<length; i++){
    equal = equal && (s1[i]==s2[i]);
  }
  return equal;
}