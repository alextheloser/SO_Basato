#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

/*#define MAXX 80
#define MAXY 24*/ //prendo le dimensioni della finestra del terminale nel main
#define PASSO 1

typedef enum {Navicella, Nemico, NemicoAvanzato, Missile, Bomba}identity;

typedef struct{
    int x;
    int y;
    int x_missile[2]; //i nemici useranno solo la prima locazione (hanno solo una bomba ciascuno)
    int y_missile[2];
    identity i;
    int id; //solo per i nemici
}Position;

void navicella(int pipeout, int maxx, int maxy);
void nemiciPrimoLivello(int pipeout, int x, int y, int idNemico, int maxx, int maxy);
void controllo(int pipein, int maxx, int maxy);
void missile();

int numNemici=10;

char SpriteNavicella[6][6]={
        "---",
        " | >",
        "---"};

char SpriteNemicoBase[4][4]={
        " /\\",
        "<OO",
        " \\/"};

int main() {
    int filedes[2], i, maxx, maxy;
    pid_t pid_navicella, pid_nemici[numNemici];

    initscr();
    noecho();
    keypad(stdscr, 1); //funzione per leggere i tasti della tastiera
    curs_set(0);
    getmaxyx(stdscr, maxy, maxx);
    srand((int)time(NULL));


    if(pipe(filedes) == -1){
        perror("Errore nella creazione della pipe!");
        _exit(1);
    }

    for(i=0; i<numNemici; i++){
        pid_nemici[i]=fork();
        switch(pid_nemici[i]){
            case -1:
                perror("Errore nell'esecuzione della fork!");
                exit(1);
            case 0:
                close(filedes[0]);
                nemiciPrimoLivello(filedes[1], 11+4*i, 1+3*i, i, maxx, maxy);
            default:
                break;
        }
    }
    pid_navicella=fork();
    switch(pid_navicella){
        case -1:
            perror("Errore nell'esecuzione della fork!");
            exit(1);
        case 0:
            close(filedes[0]);
            navicella(filedes[1], maxx, maxy);
        default:
            close(filedes[1]);
            controllo(filedes[0], maxx, maxy);
    }

    for(i=0; i<numNemici; i++){
        kill(pid_nemici[i], 1);
    }
    kill(pid_navicella,1);
    endwin();
    return 0;
}

void navicella(int pipeout, int maxx, int maxy){
    Position pos_navicella;
    pos_navicella.x=1;
    pos_navicella.y=2;
    pos_navicella.i=Navicella;
    pid_t pid_missile;

    write(pipeout, &pos_navicella, sizeof(pos_navicella));

    int c;

    while(1) {
        c=getch();
        switch(c){
            case KEY_UP:
                if(pos_navicella.y>2){
                    pos_navicella.y--;
                }
                break;
            case KEY_DOWN:
                if(pos_navicella.y<maxy-3){
                    pos_navicella.y++;
                }
                break;
            case ' ':
                pid_missile=fork();
                switch(pid_missile){
                    case -1:
                        perror("Errore nell'esecuzione della fork!");
                        exit(1);
                    case 0:
                        missile();

                }
        }
        write(pipeout,&pos_navicella,sizeof(pos_navicella));
    }
}

void nemiciPrimoLivello(int pipeout, int x, int y, int idNemico, int maxx, int maxy){
    Position pos_nemico;
    pos_nemico.x=x;
    pos_nemico.y=y;
    pos_nemico.i=Nemico;
    pos_nemico.id=idNemico;
    int r, dirx, diry;

    write(pipeout, &pos_nemico, sizeof(pos_nemico));
    //rifare movimento delle navicelle nemiche!!!!!
    while(1){
        r=random();
        if(r<RAND_MAX/2)
            dirx=1;
        else
            dirx=-1;
        if(pos_nemico.x+dirx<3 || pos_nemico.x+dirx>=maxx)
            dirx=-dirx;
        pos_nemico.x+=dirx;

        r=random();
        if(r<RAND_MAX/2)
            diry=PASSO;
        else
            diry=-PASSO;
        if(pos_nemico.y+diry<2 || pos_nemico.y+diry>=maxy)
            diry=-diry;
        pos_nemico.y+=diry;

        write(pipeout,&pos_nemico,sizeof(pos_nemico));
        usleep(1000000);
    }
}

void controllo(int pipein, int maxx, int maxy){
    Position nemico[numNemici], navicella, valore_letto;
    navicella.x=-1;
    int i, vite=3;
    for(i=0; i<numNemici; i++){
        nemico[i].x=-1;
    }
    mvprintw(0, 1, "Vite: %d", vite);
    for(i=0; i<maxx; i++){
        mvprintw(1, i, "-");
    }
    do{
        read(pipein, &valore_letto, sizeof(valore_letto));

        switch (valore_letto.i) {
            case Nemico:
                for (i = 0; i < 3; i++) {
                    mvprintw(nemico[valore_letto.id].y + i, nemico[valore_letto.id].x, "    ");
                }
                nemico[valore_letto.id] = valore_letto;
                for (i= 0; i < 3; i++) {
                    mvprintw(nemico[valore_letto.id].y + i, nemico[valore_letto.id].x, SpriteNemicoBase[i]);
                }
                break;
            case Navicella:
                for (i = 0; i < 3; i++) {
                    mvprintw(navicella.y + i, navicella.x, "    ");
                }
                navicella = valore_letto;
                for(i=0; i<3; i++){
                    mvprintw(navicella.y+i, navicella.x, SpriteNavicella[i]);
                }
                break;
        }
        mvprintw(0, 1, "Vite: %d", vite);
        for(i=0; i<maxx; i++){
            mvprintw(1, i, "-");
        }
        refresh();

    } while(1);
}

void missile(){

}