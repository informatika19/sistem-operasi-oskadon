#include "math.h"


int mod(int a, int b){
  int neg = 0;
  int i = 0;
  if(b==0){
    return 0;
  }
  //boolean to determine result is negative or not
  if(a<0){
    neg = !neg;
    a = -a;
  }
  if(b<0){
    b = -b;
  }
  //division using binary lifting
  while(b+b <= a){
    b = b+b;
    i++;
  }
  while(i>=0){
    if(b<=a){
      a -= b;
    }
    b >>= 1;
    i--;
  }
  if(neg){
    return -a;
  }else{
    return a;
  }
}

int div(int a, int b){
  int neg = 0;
  int i = 0;
  int add = 1;
  int result = 0;
  if(b==0){
    return 0;
  }
  //boolean to determine result is negative or not
  if(a<0){
    neg = !neg;
    a = -a;
  }
  if(b<0){
    neg = !neg;
    b = -b;
  }
  //division using binary lifting
  while(b+b <= a){
    b = b+b;
    add = add + add;
    i++;
  }
  while(i>=0){
    if(b<=a){
      a -= b;
      result += add;
    }
    b >>= 1;
    add >>= 1;
    i--;
  }
  if(neg){
    return -result;
  }else{
    return result;
  }
}