#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

void nemici(int pipeout, int x, int y, int idNemico, int maxx, int maxy);
void navicella(int pipeout, int maxx, int maxy);
void controllo(int pipein, int maxx, int maxy, int numNemici);

int main() {
    int filedes[2], i, j=0, maxx, maxy, x_nemici, y_nemici, numColonne=1, numNemici=6;;
    pid_t pid_navicella, pid_nemici[numNemici];

    initscr();
    noecho();
    cbreak();
    keypad(stdscr, 1); //funzione per leggere i tasti della tastiera
    curs_set(0);
    srand((int)time(NULL));

    start_color();
    init_pair(1,COLOR_WHITE,COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_CYAN, COLOR_BLACK);
    attron(COLOR_PAIR(1));

    getmaxyx(stdscr, maxy, maxx);
    x_nemici=maxx-7;
    y_nemici=3;

    refresh();
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
                nemici(filedes[1], x_nemici, y_nemici, i, maxx, maxy);
            default:
                break;
        }


        j++;

        //y_nemici=(y_nemici+9)%maxy;
        y_nemici=(y_nemici+15)%maxy;
        if(j>3 || y_nemici+7>maxy){
            j=0;
            x_nemici-=9;
            if(numColonne%2==0)
                y_nemici=1;
            else
                //y_nemici=2;
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
            navicella(filedes[1], maxx, maxy);
        default:
            close(filedes[1]);
            controllo(filedes[0], maxx, maxy, numNemici);
    }

    for(i=0; i<numNemici; i++){
        kill(pid_nemici[i], 1);
    }
    kill(pid_navicella,1);
    endwin();
    return 0;
}
