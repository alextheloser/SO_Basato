#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define MAXX 80
#define MAXY 24
#define PASSO 1

typedef enum {Navicella, Nemico, NemicoAvanzato}identity;

typedef struct{
    int x;
    int y;
    identity i;
}Position;

//Ciao andrea sto provando a fare un commit da un'altra macchina virtuale :)
//Ciao ale sto sitemando il movimento della navicella 8==)
//pisellopalleabnormi
void navicella(int pipeout);
void nemiciPrimoLivello(int pipeout, int x, int y);
void controllo(int pipein);

char SpriteNavicella[6][6]= {
        "---",
        " | >",
        "---"};

char SpriteNemicoBase[4][4]={
        " /\\",
        "<OO",
        " \\/"};
  //yes concordo un array di piedi
  //feetallora direi di fare un array di pid delle navicelle nemiche
  //fanno cogmpagnia lasciamoli
  //mmmh però come facciamo per la generazione dei processi?
  //eja, comunque ho fatto, ora provo a vedere se i due processi li gestisce così
  //vedo solo una navicella

int main() {
    int filedes[2], numNemici=2;
    pid_t pid_navicella, pid_nemici[numNemici];

    initscr();
    noecho();
    keypad(stdscr, 1); //funzione per leggere i tasti della tastiera
    curs_set(0);
    srand((int)time(0));


    if(pipe(filedes) == -1){
        perror("Errore nella creazione della pipe!");
        _exit(1);
    }

    pid_nemici[0]=fork();
    switch(pid_nemici[0])
    {
        case -1:
            perror("Errore nell'esecuzione della fork.");
            exit(1);
        case 0:
            close(filedes[0]);
            nemiciPrimoLivello(filedes[1], rand()%MAXX, rand()%MAXY);
        default:
            pid_nemici[1]=fork();
            switch(pid_nemici[1])
            {
                case -1:
                    perror("Errore nell'esecuzione della fork.");
                    exit(1);
                case 0:
                    close(filedes[0]);
                    nemiciPrimoLivello(filedes[1], rand()%MAXX, rand()%MAXY);
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
    }

    kill(pid_nemici[0],1);
    kill(pid_nemici[1],1);
    kill(pid_navicella,1);
    endwin();
    return 0;
}

void navicella(int pipeout){
    Position pos_navicella;
    pos_navicella.x=1;
    pos_navicella.y=1;
    pos_navicella.i=Navicella;

    write(pipeout, &pos_navicella, sizeof(pos_navicella));

    int c;

    while(1) {
        c=getch();
        switch(c){
            case KEY_UP:
                if(pos_navicella.y>0){
                    pos_navicella.y--;
                }
                break;
            case KEY_DOWN:
                if(pos_navicella.y<MAXY-3){
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
    pos_nemico.i=Nemico;
    int r, dirx, diry;

    write(pipeout, &pos_nemico, sizeof(pos_nemico));

    while(1){
        r=random();
        if(r<RAND_MAX/2)
            dirx=1;
        else
            dirx=-1;
        if(pos_nemico.x+dirx<1 || pos_nemico.x+dirx>=MAXX)
            dirx=-dirx;
        pos_nemico.x+=dirx;

        r=random();
        if(r<RAND_MAX/2)
            diry=PASSO;
        else
            diry=-PASSO;
        if(pos_nemico.y+diry<1 || pos_nemico.y+diry>=MAXY)
            diry=-diry;
        pos_nemico.y+=diry;

        write(pipeout,&pos_nemico,sizeof(pos_nemico));
        usleep(1000000);
    }
}

void controllo(int pipein){
    Position nemico, navicella, valore_letto;
    navicella.x=-1;
    nemico.x=-1;
    int i;
    do{
        read(pipein, &valore_letto, sizeof(valore_letto));

        if(valore_letto.i==Nemico){
            if(nemico.x >= 0 ){
                for(i=0; i<3; i++){
                    mvprintw(nemico.y+i, nemico.x, "      ");
                }
            }
            nemico=valore_letto;
        }
        else {
            if (navicella.x >= 0) {
                for (i = 0; i < 3; i++) {
                    mvprintw(navicella.y + i, navicella.x, "    ");
                }
            }
            navicella = valore_letto;
        }

        if(valore_letto.i==Nemico){
            for(i=0; i<3; i++){
                mvprintw(nemico.y+i, nemico.x, SpriteNemicoBase[i]);
            }
        }
        else{
            for(i=0; i<3; i++){
                mvprintw(navicella.y+i, navicella.x, SpriteNavicella[i]);
            }
        }
        refresh();

    } while(navicella.x != nemico.x || navicella.y != nemico.y);
}