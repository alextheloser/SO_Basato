#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define PASSO 1
#define DELAY_NEMICI 1200000
#define DELAY_BOMBA 20000

typedef enum {Navicella, Nemico, Missile, Bomba, BombaAvanzata}identity;

typedef struct{
    int x;
    int y;
    identity i;
    int id; //solo per i nemici
    pid_t pid;
}Position;

void nemici(int pipeout, int x, int y, int idNemico, int maxx, int maxy);
void bomba(int pipeout, int x_bomba, int y_bomba, int id, int type);
