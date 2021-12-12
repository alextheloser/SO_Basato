#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "navicella.h"
#include "engine.h"


int main() {
    printf("Hello, World!\n");
    int a, i;
    for(i=0; i<3; i++){
        printf("%s", SpriteNavicella[i]);
    }
    return 0;
}

