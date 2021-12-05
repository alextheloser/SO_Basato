#include <stdio.h>
#include "navicella.h"

int main() {
    printf("Hello, World!\n");
    int a, i;
    a=1+2+3;
    for(i=0; i<3; i++){
        printf("%s", SpriteNavicella[i]);
    }

    return 0;
}


