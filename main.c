#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

int menu(int maxx, int maxy);
void navicella(int pipeout, int maxx, int maxy);
void nemici(int pipeout, int x, int y, int idNemico, int maxx, int maxy);
void controllo(int pipein, int maxx, int maxy, int numNemici, int valoreDifficolta);

/**
 * Funzione principale nel quale vengono generati i processi.
 */
int main() {
    int filedes[2], i, j=0, maxx, maxy, x_nemici, y_nemici, numColonne=1, numNemici;
    numNemici=0;
    srand((int)time(NULL));

    //impostazioni dello schermo.
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, 1); //funzione per leggere i tasti della tastiera.
    curs_set(0);

    //impostazioni dei colori.
    start_color();
    init_color(80, 255, 0, 0);
    init_pair(1,COLOR_WHITE,COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_CYAN, COLOR_BLACK);
    attron(COLOR_PAIR(1));

    //viene presa la dimensione delo schermo.
    getmaxyx(stdscr, maxy, maxx);
    x_nemici=maxx-7;
    y_nemici=3;

    //menù iniziale.
    int valoreDifficolta = menu(maxx,maxy);

    clear();
    refresh();

    //in base al valore di ritorno del menù viene impostato il numero di nemici.
    switch(valoreDifficolta){
        case 0:
            numNemici=5;
            break;

        case 1:
            numNemici=10;
            break;

        case 2:
            numNemici=15;
            break;

        case 3:
            numNemici=0;
            break;
    }

    pid_t pid_navicella, pid_nemici[numNemici];

    //creazione della pipe con gestione dell'errore.
    if(pipe(filedes) == -1){
        perror("Errore nella creazione della pipe!");
        _exit(1);
    }

    //generazione dei processi che si occupano della generazione delle coordinate dei nemici.
    for(i=0; i<numNemici; i++){
        pid_nemici[i]=fork();
        switch(pid_nemici[i]){
            case -1:
                perror("Errore nell'esecuzione della fork!");
                exit(1);
            case 0:
                close(filedes[0]);
                nemici(filedes[1], x_nemici, y_nemici, i, maxx, maxy);
            default:
                break;
        }

        j++;
        //aggiornamento delle coordinate iniziali del prossimo nemico da generare.
        y_nemici=y_nemici+15;
        if(j>3 || y_nemici+7>maxy){
            j=0;
            x_nemici-=9;
            if(numColonne%2==0)
                y_nemici=3;
            else
                y_nemici=10;
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
            //creazione del processo che si occupa di gestire la navicella.
            navicella(filedes[1], maxx, maxy);
        default:
            close(filedes[1]);
            //il processo padre entra nella funzione controllo.
            controllo(filedes[0], maxx, maxy, numNemici, valoreDifficolta);
    }

    //terminazione dei processi.
    for(i=0; i<numNemici; i++){
        kill(pid_nemici[i], 1);
    }
    kill(pid_navicella,1);
    //chiusura della finestra.
    endwin();
    return 0;
}
