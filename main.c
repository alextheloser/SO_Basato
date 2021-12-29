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
void missile(int pipeout, int maxx, int maxy, int navx, int navy, int pidMissile, int *isMissileVivo);

int numNemici=10;

char SpriteNavicella[6][6]={
        "---",
        " | >",
        "---"};

char SpriteNemicoBase[4][4]={
        " /\\",
        "<OO",
        " \\/"};

char SpriteMissile='-';

int main() {
    int filedes[2], i, j=0, maxx, maxy, x_nemici, y_nemici, numColonne=1;
    pid_t pid_navicella, pid_nemici[numNemici];

    initscr();
    noecho();
    cbreak();
    keypad(stdscr, 1); //funzione per leggere i tasti della tastiera
    curs_set(0);
    srand((int)time(NULL));

    getmaxyx(stdscr, maxy, maxx);
    x_nemici=maxx-5;
    y_nemici=3;

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
                nemiciPrimoLivello(filedes[1], x_nemici, y_nemici, i, maxx, maxy);
            default:
                break;
        }
        j++;
        y_nemici=(y_nemici+6)%maxy;
        if(j>9){
            j=0;
            x_nemici-=4;
            if(numColonne%2==0)
                y_nemici=3;
            else
                y_nemici=6;
            numColonne++;
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

/**
 * Funzione che si occupa di generare le coordinate della navicella
 * @param pipeout File descriptor in scrittura della pipe
 * @param maxx Massimo valore delle X sullo schermo
 * @param maxy Massimo valore delle Y sullo schermo
 */
void navicella(int pipeout, int maxx, int maxy){
    Position pos_navicella;
    pos_navicella.x=1;
    pos_navicella.y=2;
    pos_navicella.i=Navicella;
    pid_t pid_missile;
    int isMissileVivo=0;

    write(pipeout, &pos_navicella, sizeof(pos_navicella));

    int c;

    while(1) {
        c=getch();
        switch(c) {
            case KEY_UP:
                if (pos_navicella.y > 2) {
                    pos_navicella.y--;
                }
                break;
            case KEY_DOWN:
                if (pos_navicella.y < maxy - 3) {
                    pos_navicella.y++;
                }
                break;
            case ' ':
                if (isMissileVivo == 0) {
                    isMissileVivo=1;

                    //waitpid(pid_missile, &sig, WNOHANG);
                    pid_missile = fork();
                    switch (pid_missile) {
                        case -1:
                            perror("Errore nell'esecuzione della fork!");
                            exit(1);
                        case 0:
                            missile(pipeout, maxx, maxy, pos_navicella.x, pos_navicella.y, pid_missile, &isMissileVivo);
                            //isMissileVivo=0;
                        default:
                            break;
                    }


                }
                int sig;
                waitpid(pid_missile, &sig, WNOHANG);
                //kill(pid_missile,1);
                break;
        }
        write(pipeout,&pos_navicella,sizeof(pos_navicella));

    }
}

/**
 * Funzione che si occupa di generare le coordinate di una navicella nemica
 * @param pipeout File descriptor in scrittura della pipe
 * @param x Coordinata X iniziale del nemico
 * @param y Coordinata Y iniziale del nemico
 * @param idNemico Numero identificativo della navicella nemica
 * @param maxx Massimo valore delle X sullo schermo
 * @param maxy Massimo valore delle Y sullo schermo
 */
void nemiciPrimoLivello(int pipeout, int x, int y, int idNemico, int maxx, int maxy){
    Position pos_nemico;
    pos_nemico.x=x;
    pos_nemico.y=y;
    pos_nemico.i=Nemico;
    pos_nemico.id=idNemico;
    int r=1, dirx, diry;

    write(pipeout, &pos_nemico, sizeof(pos_nemico));
    //rifare movimento delle navicelle nemiche!!!!!
    while(1){
        dirx=-PASSO;
        pos_nemico.x+=dirx;

        if(r%2==0)
            diry=PASSO;
        else
            diry=-PASSO;
        if(pos_nemico.y+diry<2 || pos_nemico.y+diry>=maxy)
            diry=-diry;
        pos_nemico.y+=diry;
        r++;

        write(pipeout,&pos_nemico,sizeof(pos_nemico));
        usleep(1000000);
    }
}

/**
 * Funzione che si occupa della stampa dei vari elementi a schermo e delle collisioni
 * @param pipein File descriptor in lettura della pipe
 * @param maxx Massimo valore delle X sullo schermo
 * @param maxy Massimo valore delle Y sullo schermo
 */
void controllo(int pipein, int maxx, int maxy){
    Position nemico[numNemici], navicella, valore_letto, missile;
    navicella.x=-1;
    int i, vite=3;
    for(i=0; i<numNemici; i++){
        nemico[i].x=-1;
    }
    //stampa vite
    mvprintw(0, 1, "Vite: %d", vite);
    for(i=0; i<maxx; i++){
        mvprintw(1, i, "-");
    }
    do{
        //leggo un valore dalla pipe
        read(pipein, &valore_letto, sizeof(valore_letto));

        //controllo che tipo di valore ho letto
        switch (valore_letto.i) {
            case Nemico:
                //elimino il nemico dalle coordinate vecchie
                for (i = 0; i < 3; i++) {
                    mvprintw(nemico[valore_letto.id].y + i, nemico[valore_letto.id].x, "    ");
                }
                //aggiorno le coordinate del nemico
                nemico[valore_letto.id] = valore_letto;
                //stampo il nemico
                for (i= 0; i < 3; i++) {
                    mvprintw(nemico[valore_letto.id].y + i, nemico[valore_letto.id].x, SpriteNemicoBase[i]);
                }
                break;
            case Navicella:
                //elimino la navicella dalle coordinate vecchie
                for (i = 0; i < 3; i++) {
                    mvprintw(navicella.y + i, navicella.x, "    ");
                }
                //aggiorno le coordinate della navicella
                navicella = valore_letto;
                //stampo la navicella
                for(i=0; i<3; i++){
                    mvprintw(navicella.y+i, navicella.x, SpriteNavicella[i]);
                }
                break;

            case Missile:
                //elimino la navicella dalle coordinate vecchie
                mvaddch(missile.y, missile.x,' ');

                //aggiorno le coordinate della navicella
                missile = valore_letto;
                //stampo la navicella
                mvaddch(missile.y, missile.x,SpriteMissile);
                break;
        }
        //stampa vite
        mvprintw(0, 1, "Vite: %d", vite);
        for(i=0; i<maxx; i++){
            mvprintw(1, i, "-");
        }
        refresh();

    } while(1);
}

void missile(int pipeout, int maxx, int maxy, int navx, int navy, int pidMissile, int *isMissileVivo){

    Position pos_missile;
    pos_missile.x=4+navx;
    pos_missile.y=1+navy;
    pos_missile.i=Missile;
    int diry=1;

    write(pipeout, &pos_missile, sizeof(pos_missile));
    while(!(pos_missile.x>maxx)){
        if(pos_missile.y+diry>maxy || pos_missile.y+diry<3) {diry=-diry;}
        pos_missile.y+=diry;
        pos_missile.x++;
        write(pipeout, &pos_missile, sizeof(pos_missile));
        usleep(30000);
    }
    *isMissileVivo=0;
    //kill(getpid(),SIGKILL);
    exit(1);
}