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

void controllo(int pipein, int maxx, int maxy, int numNemici, int valoreDifficolta);
int menu(int maxx, int maxy);

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

char logo[8][150]={".d88888b                                      888888ba           .8888b                         dP                            ",
              "88.    \"'                                     88    `8b          88   \"                         88                            ",
              "`Y88888b. 88d888b. .d8888b. .d8888b. .d8888b. 88     88 .d8888b. 88aaa  .d8888b. 88d888b. .d888b88 .d8888b. 88d888b. .d8888b. ",
              "      `8b 88'  `88 88'  `88 88'  `\"\" 88ooood8 88     88 88ooood8 88     88ooood8 88'  `88 88'  `88 88ooood8 88'  `88 Y8ooooo. ",
              "d8'   .8P 88.  .88 88.  .88 88.  ... 88.  ... 88    .8P 88.  ... 88     88.  ... 88    88 88.  .88 88.  ... 88             88 ",
              " Y88888P  88Y888P' `88888P8 `88888P' `88888P' 8888888P  `88888P' dP     `88888P' dP    dP `88888P8 `88888P' dP       `88888P' ",
              "          88                                                                                                                  ",
              "          dP                                                                                                                  "};

char gameover[7][100]={"  .oooooo.                                               .oooooo.                                  ",
                      " d8P'  `Y8b                                             d8P'  `Y8b                                 ",
                      "888            .oooo.   ooo. .oo.  .oo.    .ooooo.     888      888 oooo    ooo  .ooooo.  oooo d8b ",
                      "888           `P  )88b  `888P\"Y88bP\"Y88b  d88' `88b    888      888  `88.  .8'  d88' `88b `888\"\"8P ",
                      "888     ooooo  .oP\"888   888   888   888  888ooo888    888      888   `88..8'   888ooo888  888     ",
                      "`88.    .88'  d8(  888   888   888   888  888    .o    `88b    d88'    `888'    888    .o  888     ",
                      " `Y8bood8P'   `Y888\"\"8o o888o o888o o888o `Y8bod8P'     `Y8bood8P'      `8'     `Y8bod8P' d888b    "};

char youwon[7][100]={"  .oooooo.                                               .oooooo.                                  ",
                    " d8P'  `Y8b                                             d8P'  `Y8b                                 ",
                    "888            .oooo.   ooo. .oo.  .oo.    .ooooo.     888      888 oooo    ooo  .ooooo.  oooo d8b ",
                    "888           `P  )88b  `888P\"Y88bP\"Y88b  d88' `88b    888      888  `88.  .8'  d88' `88b `888\"\"8P ",
                    "888     ooooo  .oP\"888   888   888   888  888ooo888    888      888   `88..8'   888ooo888  888     ",
                    "`88.    .88'  d8(  888   888   888   888  888    .o    `88b    d88'    `888'    888    .o  888     ",
                    " `Y8bood8P'   `Y888\"\"8o o888o o888o o888o `Y8bod8P'     `Y8bood8P'      `8'     `Y8bod8P' d888b    "};
