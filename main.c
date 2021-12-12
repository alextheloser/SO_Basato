#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

/*#define KEY_UP 65
#define KEY_DOWN 66*/
#define MAXX 80
#define MAXY 24

typedef enum {Navicella, Nemico, NemicoAvanzato}identity;

typedef struct{
    identity i;
    int x;
    int y;
    char* sprite;
}Position;

void navicella(int pipeout);

char SpriteNavicella[]={"---\n | >\n---\n"};

char SpriteNemicoBase[]={"  |.... \n< OO \n  |____"};
/*
---
 | >
---
*/


int main() {
    int filedes[2];
    pid_t pid_navicella, pid_nemici;
    initscr();
    noecho();
    srand((int)time(0));
    curs_set(0);

    if(pipe(filedes) == -1){
        perror("Errore nella creazione della pipe!");
        _exit(1);
    }

    pid_nemici=fork();
    switch(pid_nemici)
    {
        case -1:
            perror("Errore nell'esecuzione della fork.");
            exit(1);
        case 0:
            close(filedes[0]);
            //nemici(filedes[1]);
        default:
            pid_navicella=fork();
            switch(pid_navicella)
            {
                case -1:
                    perror("Errore nell'esecuzione della fork.");
                    _exit(1);
                case 0:
                    close(filedes[0]);
                    navicella(filedes[1]);
                default:
                    close(filedes[1]);
                    controllo(filedes[0]);
            }
    }

    kill(pid_nemici,1);
    kill(pid_navicella,1);
    endwin();
    return 0;
}

void navicella(int pipeout){
    Position pos_navicella;
    pos_navicella.x=1;
    pos_navicella.y=1;
    pos_navicella.sprite=SpriteNavicella;
    pos_navicella.i=Navicella;

    write(pipeout, &pos_navicella, sizeof(pos_navicella));

    while(1) {
        char c;
        switch(c=getch()){
            case KEY_UP:
                if(pos_navicella.y>0){
                    pos_navicella.y--;
                }
                break;
            case KEY_DOWN:
                if(pos_navicella.y<MAXY-1){
                    pos_navicella.y++;
                }
                break;
        }
        write(pipeout,&pos_navicella,sizeof(pos_navicella));
    }
}

void nemiciPrimoLivello(int pipeout, int x, int y){
    Position pos_nemico;
    pos_nemico.x=x;
    pos_nemico.y=y;
    pos_nemico.sprite=SpriteNemicoBase;
    pos_nemico.i=Nemico;
    int r, dirx=1, diry=1;

    write(pipeout, &pos_nemico, sizeof(pos_nemico));

    while(1){
        r=random();
        if(r<RAND_MAX/2)
            dx=1;
        else
            dx=-1;
        if(pos_nemico.x+dx<1 || pos_nemico.x+dx>=MAXX)
            dx=-dx;
        pos_nemico.x+=dx;

        r=random();
        if(r<RAND_MAX/2)
            dy=PASSO;
        else
            dy=-PASSO;
        if(pos_nemico.y+dy<1 || pos_nemico.y+dy>=MAXY)
            dy=-dy;
        pos_nemico.y+=dy;

        write(pipeout,&pos_nemico,sizeof(pos_nemico));
        usleep(100000);
    }
}

void controllo(int pipein){
    Position nemico, navicella, valore_letto;
    navicella.x=-1;
    nemico.x=-1;

    do{
        read(pipein, &valore_letto, sizeof(valore_letto));
        if(valore_letto.i==Nemico){
            if(nemico.x >= 0 ){

                mvprintw(nemico.y, nemico.x, "     \n     \n     ");
            }
            nemico=valore_letto;
        }
        else{
            if(navicella.x >= 0 ){
                mvprintw(navicella.y, navicella.x, "   \n    \n   \n");
                //mvprintw(navicella.y, navicella.x, "PISELLO");
            }
            navicella=valore_letto;
        }
        mvprintw(valore_letto.y, valore_letto.x, valore_letto.sprite);
        curs_set(0);
        refresh();
    } while(navicella.x != nemico.x || navicella.y != nemico.y);
}

