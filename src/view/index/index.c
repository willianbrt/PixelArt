#include <stdio.h>
#include <emscripten.h>

EMSCRIPTEN_KEEPALIVE
int soma(int first, int second){
    return first + second;
}

EMSCRIPTEN_KEEPALIVE
int main() {
    printf("CONSOLE PELO PRINTF!\n");
    return 0;
}