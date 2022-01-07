#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define PASSO 1

typedef enum {Navicella, Nemico, Missile, Bomba, BombaAvanzata}identity;

typedef struct{
    int x;
    int y;
    identity i;
    int id; //solo per i nemici
    pid_t pid;
}Position;

void controllo(int pipein, int maxx, int maxy, int numNemici);

//sprite vari usati per la stampa.
char SpriteNavicella[6][6]={
        "<-\\",
        "~[-->",
        "<-/"};

char SpriteNavicellaDmgd2[6][6]={
        "<X\\",
        "-[-->",
        "<X/"};

char SpriteNavicellaDmgd3[6][6]={
        "<X\\",
        ":[XX>",
        "<X/"};

char SpriteNavicellaMorta[6][6]={
        "XX\\",
        "XXXXx",
        "XX/"};

char SpriteNemicoBase[4][4]={
        " /\\",
        "<O(",
        " \\/"};

char SpriteNemicoMorente[4][4]={
        " xx",
        "xxx",
        " xx"};

char SpriteNemicoAvanzato[4][4]={
        " /",
        "<:(",
        " \\"};

char SpriteMissile='>';

char SpriteBomba='@';
