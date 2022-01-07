#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define PASSO 1
#define DELAY_MISSILE 10000

typedef enum {Navicella, Nemico, Missile, Bomba, BombaAvanzata}identity;

typedef struct{
    int x;
    int y;
    identity i;
    int id; //solo per i nemici
    pid_t pid;
}Position;

void navicella(int pipeout, int maxx, int maxy);
void missile(int pipeout, int maxx, int maxy, int navx, int navy, int diry);
