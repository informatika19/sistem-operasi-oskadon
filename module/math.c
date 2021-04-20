#include "math.h"

int mod(int a, int b) {
    while (a >= b) {
        a -= b;
    }
    return a;
}

int div(int a, int b) {
    int res = 0;
    while (a >= b) {
        a -= b;
        res++;
    }
    return res;
}