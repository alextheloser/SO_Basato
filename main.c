#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define PASSO 1

typedef enum {Navicella, Nemico, Missile, Bomba}identity;

typedef struct{
    int x;
    int y;
    identity i;
    int id; //solo per i nemici
    pid_t pid;
}Position;


int menu();
void navicella(int pipeout, int maxx, int maxy);
void nemiciPrimoLivello(int pipeout, int x, int y, int idNemico, int maxx, int maxy);
void nemiciSecondoLivello(int pipeout, int x, int y, int idNemico, int maxx, int maxy);
void controllo(int pipein, int maxx, int maxy);
void missile(int pipeout, int maxx, int maxy, int navx, int navy, int diry);
void bomba(int pipeout, int x_bomba, int y_bomba, int id);

int numNemici=30;
int nemiciVivi;

char SpriteNavicella[6][6]={
        "<-\\",
        "~[-->",
        "<-/"};

char SpriteNavicellaDmgd2[6][6]={
        "<X\\",
        "-[-->",
        "<X/"};

char SpriteNavicellaDmgd3[6][6]={
        "<X\\",#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define PASSO 1

typedef enum {Navicella, Nemico, Missile, Bomba}identity;

typedef struct{
    int x;
    int y;
    identity i;
    int id; //solo per i nemici
    pid_t pid;
}Position;


int menu();
void navicella(int pipeout, int maxx, int maxy);
void nemiciPrimoLivello(int pipeout, int x, int y, int idNemico, int maxx, int maxy);
void nemiciSecondoLivello(int pipeout, int x, int y, int idNemico, int maxx, int maxy);
void controllo(int pipein, int maxx, int maxy);
void missile(int pipeout, int maxx, int maxy, int navx, int navy, int diry);
void bomba(int pipeout, int x_bomba, int y_bomba, int id);

int numNemici=2;

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
        "<OO",
        " \\/"};

char SpriteNemicoMorente[4][4]={
        " xx",
        "xxx",
        " xx"};

/*char SpriteNemicoAvanzato[4][4]={
        "0 1",
        "   ",
        "2 3"};*/

char SpriteNemicoAvanzato[4][4]={
        " /",
        "<:(",
        " \\"};



/*123 123
456 456
789 789
   X
123 123
456 456
789 789*/


char SpriteNemicoAvanzatoMorente[4][4]={
        "x x",
        "   ",
        "x x"};

char SpriteMissile='>';

char SpriteBomba='@';

char logo[100][10]={"   _____                      _____        __               _           \n"
                   "  / ____|                    |  __ \\      / _|             | |          \n"
                   " | (___  _ __   __ _  ___ ___| |  | | ___| |_ ___ _ __   __| | ___ _ __ \n"
                   "  \\___ \\| '_ \\ / _` |/ __/ _ \\ |  | |/ _ \\  _/ _ \\ '_ \\ / _` |/ _ \\ '__|\n"
                   "  ____) | |_) | (_| | (_|  __/ |__| |  __/ ||  __/ | | | (_| |  __/ |   \n"
                   " |_____/| .__/ \\__,_|\\___\\___|_____/ \\___|_| \\___|_| |_|\\__,_|\\___|_|   \n"
                   "        | |                                                             \n"
                   "        |_|                                                             "};

int main() {
    int filedes[2], i, j=0, maxx, maxy, x_nemici, y_nemici, numColonne=1;
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
                nemiciPrimoLivello(filedes[1], x_nemici, y_nemici, i, maxx, maxy);
            default:
                break;
        }


        j++;

        y_nemici=(y_nemici+9)%maxy;
        if(j>3 || y_nemici+3>maxy){
            j=0;
            x_nemici-=9;
            if(numColonne%2==0)
                y_nemici=1;
            else
                y_nemici=2;
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
    pos_navicella.x=2;
    pos_navicella.y=maxy/2;
    pos_navicella.i=Navicella;
    pid_t pid_missile1, pid_missile2;
    int isMissileVivo1=0, isMissileVivo2=0, sig1, sig2, c;

    write(pipeout, &pos_navicella, sizeof(pos_navicella));

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
                //if(waitpid(pid_missile1,&sig1,WNOHANG)==pid_missile1){mvprintw(10,10,"SMEGMA");}
                // if(waitpid(pid_missile2,&sig2,WNOHANG)==pid_missile2){mvprintw(14,14,"BORRA");}
                if ((isMissileVivo1==0 || isMissileVivo2==0) || (waitpid(pid_missile1,&sig1,WNOHANG)==pid_missile1 && waitpid(pid_missile2,&sig2,WNOHANG)==pid_missile2)){
                    //mvprintw(10,10,"pallatesticoli");
                    //waitpid(pid_missile, &sig, WNOHANG);
                    isMissileVivo1=1;
                    isMissileVivo2=1;
                    pid_missile1 = fork();
                    //isMissileVivo=1;
                    switch (pid_missile1) {
                        case -1:
                            perror("Errore nell'esecuzione della fork!");
                            exit(1);
                        case 0:
                            missile(pipeout, maxx, maxy, pos_navicella.x, pos_navicella.y, PASSO); //missile che va in alto all'inizio
                        default:
                            pid_missile2=fork();
                            switch(pid_missile2){
                                case -1:
                                    perror("Errore nell'esecuzione della fork!");
                                    exit(1);
                                case 0:
                                    missile(pipeout, maxx, maxy, pos_navicella.x, pos_navicella.y, -PASSO);
                                default:
                                    break;
                            }
                            /*waitpid(pid_missile1, &sig1, WNOHANG);
                            waitpid(pid_missile2, &sig2, WNOHANG);*/
                    }
                }
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
    pos_nemico.pid=getpid();
    pid_t pid_bomba;
    int r=1, dirx, diry, cicli=1;

    write(pipeout, &pos_nemico, sizeof(pos_nemico));
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

        if(!(cicli++%5)){
            pid_bomba=fork();
            switch(pid_bomba){
                case -1:
                    perror("Errore nell'esecuzione della fork!");
                    exit(1);
                case 0:
                    bomba(pipeout, pos_nemico.x-1, pos_nemico.y+1, idNemico);
                default:
                    break;
            }
        }
        write(pipeout,&pos_nemico,sizeof(pos_nemico));
        //usleep(1200000);
        usleep(800000);
    }
}
/*
void nemiciSecondoLivello(int pipeout, int x, int y, int idNemico, int maxx, int maxy){
    Position pos_nemicoAvanzato;
    pos_nemicoAvanzato.x=x;
    pos_nemicoAvanzato.y=y;
    pos_nemicoAvanzato.i=NemicoAvanzato;
    pos_nemicoAvanzato.id=idNemico;
    pos_nemicoAvanzato.pid=getpid();
    pid_t pid_bomba[2];
    int r=1, dirx, diry, cicli=1, i;

    write(pipeout, &pos_nemicoAvanzato, sizeof(pos_nemicoAvanzato));
    while(1){
        dirx=-PASSO;
        pos_nemicoAvanzato.x+=dirx;
        if(r%2==0)
            diry=PASSO;
        else
            diry=-PASSO;
        if(pos_nemicoAvanzato.y+diry<2 || pos_nemicoAvanzato.y+diry>=maxy)
            diry=-diry;
        pos_nemicoAvanzato.y+=diry;
        r++;

        if(!(cicli++%5)){
            pid_bomba[0]=fork();
            switch(pid_bomba[0]){
                case -1:
                    perror("Errore nell'esecuzione della fork!");
                    exit(1);
                case 0:
                    bomba(pipeout, pos_nemicoAvanzato.x-1, pos_nemicoAvanzato.y, idNemico);
                default:
                    pid_bomba[1]=fork();
                    switch(pid_bomba[1]){
                        case -1:
                            perror("Errore nell'esecuzione della fork!");
                            exit(1);
                        case 0:
                            bomba(pipeout, pos_nemicoAvanzato.x-1, pos_nemicoAvanzato.y+2, idNemico);
                        default:
                            break;
                    }
            }
        }
        write(pipeout,&pos_nemicoAvanzato,sizeof(pos_nemicoAvanzato));
        usleep(1200000);
    }
}
*/
/**
 * Funzione che si occupa della stampa dei vari elementi a schermo e delle collisioni
 * @param pipein File descriptor in lettura della pipe
 * @param maxx Massimo valore delle X sullo schermo
 * @param maxy Massimo valore delle Y sullo schermo
 */
void controllo(int pipein, int maxx, int maxy){

    Position nemico[numNemici], nemicoAvanzato[numNemici], bombe[numNemici], navicella, valore_letto, missili[2];
    navicella.x=-1;
    int i, vite=3, n, j, statoNemico[numNemici], vitaNemici[numNemici][4];
    for(i=0; i<numNemici; i++){
        statoNemico[i]=0;
        nemico[i].x=-1;
        bombe[i].x=-1;
        for(j=0;j<4;j++){
            vitaNemici[i][j]=1;
        }
    }
    int nemiciVivi=numNemici;
    //stampa vite
    mvprintw(0, 1, "Vite: %d", vite);
    mvprintw(0, 10, "Nemici: %d", nemiciVivi);
    for(i=0; i<maxx; i++){
        mvprintw(1, i, "-");
    }
    //refresh();
    do{
        //leggo un valore dalla pipe
        read(pipein, &valore_letto, sizeof(valore_letto));

        //controllo che tipo di valore ho letto
        switch (valore_letto.i) {
            case Nemico:
            
            attron(COLOR_PAIR(2));
                switch(statoNemico[valore_letto.id]){
                    case 0: //nemico base
                        //elimino il nemico dalle coordinate vecchie
                    //aggiungo un altro if giusto per guadagnare tempo nell'esecuzione
                    if(statoNemico[valore_letto.id]==0){
                        for (i = 0; i < 3; i++) {
                            mvprintw(nemico[valore_letto.id].y + i, nemico[valore_letto.id].x, "    ");
                        }
                        //aggiorno le coordinate del nemico
                        nemico[valore_letto.id] = valore_letto;
                        //stampo il nemico
                        for (i = 0; i < 3; i++) {
                            mvprintw(nemico[valore_letto.id].y + i, nemico[valore_letto.id].x, SpriteNemicoBase[i]);
                        }
                    }
                        break;
                    //nemico Avanzato
                    case 1:
                        if(vitaNemici[valore_letto.id][0]==1) {
                            for (i = 0; i < 3; i++) {
                                mvprintw(nemico[valore_letto.id].y + i, nemico[valore_letto.id].x, "   ");
                            }
                        }

                        if(vitaNemici[valore_letto.id][1]==1) {
                            for (i = 0; i < 3; i++) {
                                mvprintw(nemico[valore_letto.id].y + i, nemico[valore_letto.id].x + 4, "   ");
                            }
                        }

                        if(vitaNemici[valore_letto.id][2]==1) {
                            for (i = 0; i < 3; i++) {
                                mvprintw(nemico[valore_letto.id].y + i + 4, nemico[valore_letto.id].x, "   ");
                            }
                        }

                        if(vitaNemici[valore_letto.id][3]==1) {
                            for (i = 0; i < 3; i++) {
                                mvprintw(nemico[valore_letto.id].y + i + 4, nemico[valore_letto.id].x + 4, "   ");
                            }
                        }

                        mvprintw(nemico[valore_letto.id].y+3, nemico[valore_letto.id].x+3, " ");
                        nemico[valore_letto.id] = valore_letto;
                        mvprintw(nemico[valore_letto.id].y+3, nemico[valore_letto.id].x+3, "X");

                        if(vitaNemici[valore_letto.id][0]==1) {
                            for (i = 0; i < 3; i++) {
                                mvprintw(nemico[valore_letto.id].y + i, nemico[valore_letto.id].x,
                                         SpriteNemicoAvanzato[i]);
                            }
                        }

                        if(vitaNemici[valore_letto.id][1]==1) {
                            for (i = 0; i < 3; i++) {
                                mvprintw(nemico[valore_letto.id].y + i, nemico[valore_letto.id].x + 4,
                                         SpriteNemicoAvanzato[i]);
                            }
                        }
                        if(vitaNemici[valore_letto.id][2]==1) {
                            for (i = 0; i < 3; i++) {
                                mvprintw(nemico[valore_letto.id].y + i + 4, nemico[valore_letto.id].x,
                                         SpriteNemicoAvanzato[i]);
                            }
                        }

                        if(vitaNemici[valore_letto.id][3]==1) {
                            for (i = 0; i < 3; i++) {
                                mvprintw(nemico[valore_letto.id].y + i + 4, nemico[valore_letto.id].x + 4,
                                         SpriteNemicoAvanzato[i]);
                            }
                        }
                        break;
                }
                attron(COLOR_PAIR(1));
                if(nemico[valore_letto.id].x<0){
                vite=0;
                }
                break;
            case Navicella:
                attron(COLOR_PAIR(3));
                switch(vite){
                    case 3:
                        //elimino la navicella dalle coordinate vecchie
                        for (i = 0; i < 3; i++) {
                            mvprintw(navicella.y + i, navicella.x, "     ");
                        }
                        //aggiorno le coordinate della navicella
                        navicella = valore_letto;
                        //stampo la navicella
                        for(i=0; i<3; i++){
                            mvprintw(navicella.y+i, navicella.x, SpriteNavicella[i]);
                        }
                        break;
                    case 2:
                        //elimino la navicella dalle coordinate vecchie
                        for (i = 0; i < 3; i++) {
                            mvprintw(navicella.y + i, navicella.x, "     ");
                        }
                        //aggiorno le coordinate della navicella
                        navicella = valore_letto;
                        //stampo la navicella
                        for(i=0; i<3; i++){
                            mvprintw(navicella.y+i, navicella.x, SpriteNavicellaDmgd2[i]);
                        }
                        break;
                    case 1:
                        //elimino la navicella dalle coordinate vecchie
                        for (i = 0; i < 3; i++) {
                            mvprintw(navicella.y + i, navicella.x, "     ");
                        }
                        //aggiorno le coordinate della navicella
                        navicella = valore_letto;
                        //stampo la navicella
                        for(i=0; i<3; i++){
                            mvprintw(navicella.y+i, navicella.x, SpriteNavicellaDmgd3[i]);
                        }
                        break;
                    case 0:
                        //elimino la navicella dalle coordinate vecchie
                        for (i = 0; i < 3; i++) {
                            mvprintw(navicella.y + i, navicella.x, "     ");
                        }
                        //aggiorno le coordinate della navicella
                        navicella = valore_letto;
                        //stampo la navicella
                        for(i=0; i<3; i++){
                            mvprintw(navicella.y+i, navicella.x, SpriteNavicellaMorta[i]);
                        }
                        break;
                }
                attron(COLOR_PAIR(1));
                break;

            case Missile:
                mvaddch(missili[valore_letto.id].y, missili[valore_letto.id].x,' ');
                missili[valore_letto.id] = valore_letto;
                mvaddch(missili[valore_letto.id].y, missili[valore_letto.id].x,SpriteMissile);
                for(i=0; i<numNemici; i++){
                    for(n=0; n<2; n++){
                        if(statoNemico[i]==0) {
                            if ((nemico[i].x == missili[n].x && nemico[i].y == missili[n].y) ||
                                (nemico[i].x + 1 == missili[n].x && nemico[i].y == missili[n].y) ||
                                (nemico[i].x + 2 == missili[n].x && nemico[i].y == missili[n].y)
                                || (nemico[i].x == missili[n].x && nemico[i].y + 1 == missili[n].y) ||
                                (nemico[i].x + 1 == missili[n].x && nemico[i].y + 1 == missili[n].y) ||
                                (nemico[i].x + 2 == missili[n].x && nemico[i].y + 1 == missili[n].y)
                                || (nemico[i].x == missili[n].x && nemico[i].y + 2 == missili[n].y) ||
                                (nemico[i].x + 1 == missili[n].x && nemico[i].y + 2 == missili[n].y) ||
                                (nemico[i].x + 2 == missili[n].x && nemico[i].y + 2 == missili[n].y)) {
                                
                                //mvaddch(missili[n].y, missili[n].x,'X');
                                attron(COLOR_PAIR(2));
                                mvaddch(missili[n].y, missili[n].x, ' ');
                                missili[n].x = -1;
                                missili[n].y = -1;
                                kill(missili[n].pid, 1);
                                for (j = 0; j < 3; j++) {
                                    mvprintw(nemico[i].y + j, nemico[i].x, SpriteNemicoMorente[j]);
                                }

                                refresh();
                                usleep(30000);
                                /*for (j = 0; j < 3; j++) {
                                    mvprintw(nemico[i].y + j, nemico[i].x, "    ");
                                }*/
                                attron(COLOR_PAIR(1));
                                //nemico[i].x=-4;
                                //nemico[i].y=-4;
                                //kill(nemico[i].pid, 1);

                                statoNemico[i] = 1;
                                //nemiciVivi--;
                            }
                        }
                        else if(statoNemico[i]==1){
                            //primo check enorme
                            if(vitaNemici[i][0]==1 && ((nemico[i].x == missili[n].x && nemico[i].y == missili[n].y) ||
                               (nemico[i].x + 1 == missili[n].x && nemico[i].y == missili[n].y) ||
                               (nemico[i].x + 2 == missili[n].x && nemico[i].y == missili[n].y)
                               || (nemico[i].x == missili[n].x && nemico[i].y + 1 == missili[n].y) ||
                               (nemico[i].x + 1 == missili[n].x && nemico[i].y + 1 == missili[n].y) ||
                               (nemico[i].x + 2 == missili[n].x && nemico[i].y + 1 == missili[n].y)
                               || (nemico[i].x == missili[n].x && nemico[i].y + 2 == missili[n].y) ||
                               (nemico[i].x + 1 == missili[n].x && nemico[i].y + 2 == missili[n].y) ||
                               (nemico[i].x + 2 == missili[n].x && nemico[i].y + 2 == missili[n].y))){
                                for (j = 0; j < 3; j++) {
                                    mvaddch(missili[n].y, missili[n].x, ' ');
                                    missili[n].x = -1;
                                    missili[n].y = -1;
                                    kill(missili[n].pid, 1);
                                    mvprintw(nemico[i].y + j, nemico[i].x, "   ");
                                }
                                vitaNemici[i][0]=0;
                            }
                            //secondo check enorme
                            if(vitaNemici[i][1]==1 && ((nemico[i].x + 4 == missili[n].x && nemico[i].y == missili[n].y) ||
                               (nemico[i].x + 5 == missili[n].x && nemico[i].y == missili[n].y) ||
                               (nemico[i].x + 6 == missili[n].x && nemico[i].y == missili[n].y)
                               || (nemico[i].x + 4== missili[n].x && nemico[i].y + 1 == missili[n].y) ||
                               (nemico[i].x + 5 == missili[n].x && nemico[i].y + 1 == missili[n].y) ||
                               (nemico[i].x + 6 == missili[n].x && nemico[i].y + 1 == missili[n].y)
                               || (nemico[i].x + 4== missili[n].x && nemico[i].y + 2 == missili[n].y) ||
                               (nemico[i].x + 5 == missili[n].x && nemico[i].y + 2 == missili[n].y) ||
                               (nemico[i].x + 6 == missili[n].x && nemico[i].y + 2 == missili[n].y))){
                                for (j = 0; j < 3; j++) {
                                    mvaddch(missili[n].y, missili[n].x, ' ');
                                    missili[n].x = -1;
                                    missili[n].y = -1;
                                    kill(missili[n].pid, 1);
                                    mvprintw(nemico[i].y + j, nemico[i].x + 4, "   ");
                                }
                                vitaNemici[i][1]=0;
                            }

                            if(vitaNemici[i][2]==1 && ((nemico[i].x == missili[n].x && nemico[i].y + 4 == missili[n].y) ||
                               (nemico[i].x + 1 == missili[n].x && nemico[i].y + 4 == missili[n].y) ||
                               (nemico[i].x + 2 == missili[n].x && nemico[i].y + 4== missili[n].y)
                               || (nemico[i].x == missili[n].x && nemico[i].y + 5 == missili[n].y) ||
                               (nemico[i].x + 1 == missili[n].x && nemico[i].y + 5 == missili[n].y) ||
                               (nemico[i].x + 2 == missili[n].x && nemico[i].y + 5 == missili[n].y)
                               || (nemico[i].x == missili[n].x && nemico[i].y + 6 == missili[n].y) ||
                               (nemico[i].x + 1 == missili[n].x && nemico[i].y + 6 == missili[n].y) ||
                               (nemico[i].x + 2 == missili[n].x && nemico[i].y + 6 == missili[n].y))){
                                for (j = 0; j < 3; j++) {
                                    mvaddch(missili[n].y, missili[n].x, ' ');
                                    missili[n].x = -1;
                                    missili[n].y = -1;
                                    kill(missili[n].pid, 1);
                                    mvprintw(nemico[i].y + j + 4, nemico[i].x, "   ");
                                }
                                vitaNemici[i][2]=0;
                            }

                            if(vitaNemici[i][3]==1 && ((nemico[i].x + 4== missili[n].x && nemico[i].y + 4 == missili[n].y) ||
                               (nemico[i].x + 5 == missili[n].x && nemico[i].y + 4 == missili[n].y) ||
                               (nemico[i].x + 6 == missili[n].x && nemico[i].y + 4== missili[n].y)
                               || (nemico[i].x + 4== missili[n].x && nemico[i].y + 5 == missili[n].y) ||
                               (nemico[i].x + 5 == missili[n].x && nemico[i].y + 5 == missili[n].y) ||
                               (nemico[i].x + 6 == missili[n].x && nemico[i].y + 5 == missili[n].y)
                               || (nemico[i].x + 4== missili[n].x && nemico[i].y + 6 == missili[n].y) ||
                               (nemico[i].x + 5 == missili[n].x && nemico[i].y + 6 == missili[n].y) ||
                               (nemico[i].x + 6 == missili[n].x && nemico[i].y + 6 == missili[n].y))){
                                for (j = 0; j < 3; j++) {
                                    mvaddch(missili[n].y, missili[n].x, ' ');
                                    missili[n].x = -1;
                                    missili[n].y = -1;
                                    kill(missili[n].pid, 1);
                                    mvprintw(nemico[i].y + j + 4, nemico[i].x + 4, "   ");
                                }
                                vitaNemici[i][3]=0;
                            }

                            if(vitaNemici[i][0]==0 && vitaNemici[i][1]==0 && vitaNemici[i][2]==0 && vitaNemici[i][3]==0){
                                //mvaddch(missili[n].y, missili[n].x,'X');
                                mvprintw(nemico[i].y+3, nemico[i].x+3, " ");
                                nemico[i].x=-50;
                                nemico[i].y=-50;
                                /*missili[n].x = -1;
                                missili[n].y = -1;*/
                                
                                kill(nemico[i].pid, 1);
                                refresh();
                                nemiciVivi--;
                                //usleep(30000);
                                statoNemico[i]=3;

                            }
                        }
                    }
                }
                break;
            case Bomba:
                attron(COLOR_PAIR(2));
                mvaddch(bombe[valore_letto.id].y, bombe[valore_letto.id].x,' ');
                bombe[valore_letto.id] = valore_letto;
                mvaddch(bombe[valore_letto.id].y, bombe[valore_letto.id].x,SpriteBomba);
                //se la bomba tocca la navicella viene persa una vita
                for(i=0; i<numNemici; i++){
                    if((navicella.x==bombe[i].x && navicella.y==bombe[i].y) || (navicella.x+1==bombe[i].x && navicella.y==bombe[i].y) || (navicella.x+2==bombe[i].x && navicella.y==bombe[i].y)
                       || (navicella.x==bombe[i].x && navicella.y+1==bombe[i].y) || (navicella.x+1==bombe[i].x && navicella.y+1==bombe[i].y) || (navicella.x+2==bombe[i].x && navicella.y+1==bombe[i].y) || (navicella.x+3==bombe[i].x && navicella.y+1==bombe[i].y) || (navicella.x+4==bombe[i].x && navicella.y+1==bombe[i].y)
                       || (navicella.x==bombe[i].x && navicella.y+2==bombe[i].y) || (navicella.x+1==bombe[i].x && navicella.y+2==bombe[i].y) || (navicella.x+2==bombe[i].x && navicella.y+2==bombe[i].y)){
                        vite--;
                        //aggiorno le coordinate della bomba che ha toccato la navicella in modo che "scompaia"
                        bombe[i].x=-1;
                        bombe[i].y=-1;
                        //termino il processo che gestisce la bomba
                        kill(bombe[i].pid, 1);

                        int v=i;
                        i=0;
                        attron(COLOR_PAIR(3));

                        switch(vite) {

                            case 2:
                                //elimino la navicella dalle coordinate vecchie
                                for (i = 0; i < 3; i++) {
                                    mvprintw(navicella.y + i, navicella.x, "     ");
                                }
                                //stampo la navicella
                                for (i = 0; i < 3; i++) {
                                    mvprintw(navicella.y + i, navicella.x, SpriteNavicellaDmgd2[i]);
                                }
                                break;
                            case 1:
                                //elimino la navicella dalle coordinate vecchie
                                for (i = 0; i < 3; i++) {
                                    mvprintw(navicella.y + i, navicella.x, "     ");
                                }
                                //stampo la navicella
                                for (i = 0; i < 3; i++) {
                                    mvprintw(navicella.y + i, navicella.x, SpriteNavicellaDmgd3[i]);
                                }
                                break;
                        }
                        i=v;

                    }
                    
                }
                attron(COLOR_PAIR(1));
                break;
        }
        //stampa vite aggiornate
        mvprintw(0, 1, "Vite: %d", vite);
        mvprintw(0, 10, "Nemici: %d", nemiciVivi);
        for(i=0; i<maxx; i++){
            mvprintw(1, i, "-");
        }
        refresh();
    } while(vite>0 && nemiciVivi>0);
    //stampa messaggio di game over
}

void missile(int pipeout, int maxx, int maxy, int navx, int navy, int diry){

    Position pos_missile;
    pos_missile.x=5+navx;
    pos_missile.y=1+navy;
    pos_missile.i=Missile;
    pos_missile.pid=getpid();
    if(diry==1){
        pos_missile.id=0;
    }
    else{
        pos_missile.id=1;
    }
    int i=0;
    write(pipeout, &pos_missile, sizeof(pos_missile));
    while(!(pos_missile.x>maxx)){
        if(pos_missile.y+diry>maxy || pos_missile.y+diry<2) {diry=-diry;}
        if(i%6==0){
            pos_missile.y+=diry;
        }
        pos_missile.x+=1;
        write(pipeout, &pos_missile, sizeof(pos_missile));
        usleep(10000);
        i++;
    }
    //kill(getpid(),SIGKILL);
    exit(1);

}

void bomba(int pipeout, int x_bomba, int y_bomba, int id){
    Position pos_bomba;
    pos_bomba.x=x_bomba;
    pos_bomba.y=y_bomba;
    pos_bomba.i=Bomba;
    pos_bomba.id=id;
    pos_bomba.pid=getpid();

    write(pipeout, &pos_bomba, sizeof(pos_bomba));

    while(!(pos_bomba.x<0)){
        pos_bomba.x-=1;
        write(pipeout, &pos_bomba, sizeof(pos_bomba));
        usleep(30000);
    }

    exit(1);
}
        ":[XX>",
        "<X/"};

char SpriteNavicellaMorta[6][6]={
        "XX\\",
        "XXXXx",
        "XX/"};

char SpriteNemicoBase[4][4]={
        " /\\",
        "<OO",
        " \\/"};

char SpriteNemicoMorente[4][4]={
        " xx",
        "xxx",
        " xx"};

/*char SpriteNemicoAvanzato[4][4]={
        "0 1",
        "   ",
        "2 3"};*/

char SpriteNemicoAvanzato[4][4]={
        "<:(",
        "<:(",
        "<:( "};



/*123 123
456 456
789 789
   X
123 123
456 456
789 789*/


char SpriteNemicoAvanzatoMorente[4][4]={
        "x x",
        "   ",
        "x x"};

char SpriteMissile='>';

char SpriteBomba='@';

char logo[4][4]={
        "123",
        "456",
        "789"
};

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
    x_nemici=maxx-7;
    y_nemici=3;

    refresh();
    nemiciVivi=numNemici;

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
        y_nemici=(y_nemici+9)%maxy;
        if(j>9 || y_nemici+3>maxy){
            j=0;
            x_nemici-=9;
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
    pos_navicella.x=2;
    pos_navicella.y=maxy/2;
    pos_navicella.i=Navicella;
    pid_t pid_missile1, pid_missile2;
    int isMissileVivo1=0, isMissileVivo2=0, sig1, sig2, c;

    write(pipeout, &pos_navicella, sizeof(pos_navicella));

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
                //if(waitpid(pid_missile1,&sig1,WNOHANG)==pid_missile1){mvprintw(10,10,"SMEGMA");}
                // if(waitpid(pid_missile2,&sig2,WNOHANG)==pid_missile2){mvprintw(14,14,"BORRA");}
                if ((isMissileVivo1==0 || isMissileVivo2==0) || (waitpid(pid_missile1,&sig1,WNOHANG)==pid_missile1 && waitpid(pid_missile2,&sig2,WNOHANG)==pid_missile2)){
                    //mvprintw(10,10,"pallatesticoli");
                    //waitpid(pid_missile, &sig, WNOHANG);
                    isMissileVivo1=1;
                    isMissileVivo2=1;
                    pid_missile1 = fork();
                    //isMissileVivo=1;
                    switch (pid_missile1) {
                        case -1:
                            perror("Errore nell'esecuzione della fork!");
                            exit(1);
                        case 0:
                            missile(pipeout, maxx, maxy, pos_navicella.x, pos_navicella.y, PASSO); //missile che va in alto all'inizio
                        default:
                            pid_missile2=fork();
                            switch(pid_missile2){
                                case -1:
                                    perror("Errore nell'esecuzione della fork!");
                                    exit(1);
                                case 0:
                                    missile(pipeout, maxx, maxy, pos_navicella.x, pos_navicella.y, -PASSO);
                                default:
                                    break;
                            }
                            /*waitpid(pid_missile1, &sig1, WNOHANG);
                            waitpid(pid_missile2, &sig2, WNOHANG);*/
                    }
                }
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
    pos_nemico.pid=getpid();
    pid_t pid_bomba;
    int r=1, dirx, diry, cicli=1;

    write(pipeout, &pos_nemico, sizeof(pos_nemico));
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
/*
        if(!(cicli++%5)){
            pid_bomba=fork();
            switch(pid_bomba){
                case -1:
                    perror("Errore nell'esecuzione della fork!");
                    exit(1);
                case 0:
                    bomba(pipeout, pos_nemico.x-1, pos_nemico.y+1, idNemico);
                default:
                    break;
            }
        }*/
        write(pipeout,&pos_nemico,sizeof(pos_nemico));
        usleep(1200000);
    }
}
/*
void nemiciSecondoLivello(int pipeout, int x, int y, int idNemico, int maxx, int maxy){
    Position pos_nemicoAvanzato;
    pos_nemicoAvanzato.x=x;
    pos_nemicoAvanzato.y=y;
    pos_nemicoAvanzato.i=NemicoAvanzato;
    pos_nemicoAvanzato.id=idNemico;
    pos_nemicoAvanzato.pid=getpid();
    pid_t pid_bomba[2];
    int r=1, dirx, diry, cicli=1, i;

    write(pipeout, &pos_nemicoAvanzato, sizeof(pos_nemicoAvanzato));
    while(1){
        dirx=-PASSO;
        pos_nemicoAvanzato.x+=dirx;
        if(r%2==0)
            diry=PASSO;
        else
            diry=-PASSO;
        if(pos_nemicoAvanzato.y+diry<2 || pos_nemicoAvanzato.y+diry>=maxy)
            diry=-diry;
        pos_nemicoAvanzato.y+=diry;
        r++;

        if(!(cicli++%5)){
            pid_bomba[0]=fork();
            switch(pid_bomba[0]){
                case -1:
                    perror("Errore nell'esecuzione della fork!");
                    exit(1);
                case 0:
                    bomba(pipeout, pos_nemicoAvanzato.x-1, pos_nemicoAvanzato.y, idNemico);
                default:
                    pid_bomba[1]=fork();
                    switch(pid_bomba[1]){
                        case -1:
                            perror("Errore nell'esecuzione della fork!");
                            exit(1);
                        case 0:
                            bomba(pipeout, pos_nemicoAvanzato.x-1, pos_nemicoAvanzato.y+2, idNemico);
                        default:
                            break;
                    }
            }
        }
        write(pipeout,&pos_nemicoAvanzato,sizeof(pos_nemicoAvanzato));
        usleep(1200000);
    }
}
*/
/**
 * Funzione che si occupa della stampa dei vari elementi a schermo e delle collisioni
 * @param pipein File descriptor in lettura della pipe
 * @param maxx Massimo valore delle X sullo schermo
 * @param maxy Massimo valore delle Y sullo schermo
 */
void controllo(int pipein, int maxx, int maxy){

    Position nemico[numNemici], nemicoAvanzato[numNemici], bombe[numNemici], navicella, valore_letto, missili[2];
    navicella.x=-1;
    int i, vite=3, n, j, statoNemico[numNemici], vitaNemici[numNemici][4];
    for(i=0; i<numNemici; i++){
        statoNemico[i]=0;
        nemico[i].x=-1;
        bombe[i].x=-1;
        for(j=0;j<4;j++){
            vitaNemici[i][j]=1;
        }
    }

    //stampa vite
    mvprintw(0, 1, "Vite: %d", vite);
    for(i=0; i<maxx; i++){
        mvprintw(1, i, "-");
    }
    //refresh();
    do{
        //leggo un valore dalla pipe
        read(pipein, &valore_letto, sizeof(valore_letto));

        //controllo che tipo di valore ho letto
        switch (valore_letto.i) {
            case Nemico:
                switch(statoNemico[valore_letto.id]){
                    case 0: //nemico base
                        //elimino il nemico dalle coordinate vecchie
                        for (i = 0; i < 3; i++) {
                            mvprintw(nemico[valore_letto.id].y + i, nemico[valore_letto.id].x, "    ");
                        }
                        //aggiorno le coordinate del nemico
                        nemico[valore_letto.id] = valore_letto;
                        //stampo il nemico
                        for (i = 0; i < 3; i++) {
                            mvprintw(nemico[valore_letto.id].y + i, nemico[valore_letto.id].x, SpriteNemicoBase[i]);
                        }
                        break;
                    //nemico Avanzato
                    case 1:
                        if(vitaNemici[valore_letto.id][0]==1) {
                            for (i = 0; i < 3; i++) {
                                mvprintw(nemico[valore_letto.id].y + i, nemico[valore_letto.id].x, "   ");
                            }
                        }

                        if(vitaNemici[valore_letto.id][1]==1) {
                            for (i = 0; i < 3; i++) {
                                mvprintw(nemico[valore_letto.id].y + i, nemico[valore_letto.id].x + 4, "   ");
                            }
                        }

                        if(vitaNemici[valore_letto.id][2]==1) {
                            for (i = 0; i < 3; i++) {
                                mvprintw(nemico[valore_letto.id].y + i + 4, nemico[valore_letto.id].x, "   ");
                            }
                        }

                        if(vitaNemici[valore_letto.id][3]==1) {
                            for (i = 0; i < 3; i++) {
                                mvprintw(nemico[valore_letto.id].y + i + 4, nemico[valore_letto.id].x + 4, "   ");
                            }
                        }

                        mvprintw(nemico[valore_letto.id].y+3, nemico[valore_letto.id].x+3, " ");
                        nemico[valore_letto.id] = valore_letto;
                        mvprintw(nemico[valore_letto.id].y+3, nemico[valore_letto.id].x+3, "X");

                        if(vitaNemici[valore_letto.id][0]==1) {
                            for (i = 0; i < 3; i++) {
                                mvprintw(nemico[valore_letto.id].y + i, nemico[valore_letto.id].x,
                                         SpriteNemicoAvanzato[i]);
                            }
                        }

                        if(vitaNemici[valore_letto.id][1]==1) {
                            for (i = 0; i < 3; i++) {
                                mvprintw(nemico[valore_letto.id].y + i, nemico[valore_letto.id].x + 4,
                                         SpriteNemicoAvanzato[i]);
                            }
                        }
                        if(vitaNemici[valore_letto.id][2]==1) {
                            for (i = 0; i < 3; i++) {
                                mvprintw(nemico[valore_letto.id].y + i + 4, nemico[valore_letto.id].x,
                                         SpriteNemicoAvanzato[i]);
                            }
                        }

                        if(vitaNemici[valore_letto.id][3]==1) {
                            for (i = 0; i < 3; i++) {
                                mvprintw(nemico[valore_letto.id].y + i + 4, nemico[valore_letto.id].x + 4,
                                         SpriteNemicoAvanzato[i]);
                            }
                        }
                        break;
                }
                break;
            case Navicella:

                switch(vite){
                    case 3:
                        //elimino la navicella dalle coordinate vecchie
                        for (i = 0; i < 3; i++) {
                            mvprintw(navicella.y + i, navicella.x, "     ");
                        }
                        //aggiorno le coordinate della navicella
                        navicella = valore_letto;
                        //stampo la navicella
                        for(i=0; i<3; i++){
                            mvprintw(navicella.y+i, navicella.x, SpriteNavicella[i]);
                        }
                        break;
                    case 2:
                        //elimino la navicella dalle coordinate vecchie
                        for (i = 0; i < 3; i++) {
                            mvprintw(navicella.y + i, navicella.x, "     ");
                        }
                        //aggiorno le coordinate della navicella
                        navicella = valore_letto;
                        //stampo la navicella
                        for(i=0; i<3; i++){
                            mvprintw(navicella.y+i, navicella.x, SpriteNavicellaDmgd2[i]);
                        }
                        break;
                    case 1:
                        //elimino la navicella dalle coordinate vecchie
                        for (i = 0; i < 3; i++) {
                            mvprintw(navicella.y + i, navicella.x, "     ");
                        }
                        //aggiorno le coordinate della navicella
                        navicella = valore_letto;
                        //stampo la navicella
                        for(i=0; i<3; i++){
                            mvprintw(navicella.y+i, navicella.x, SpriteNavicellaDmgd3[i]);
                        }
                        break;
                    case 0:
                        //elimino la navicella dalle coordinate vecchie
                        for (i = 0; i < 3; i++) {
                            mvprintw(navicella.y + i, navicella.x, "     ");
                        }
                        //aggiorno le coordinate della navicella
                        navicella = valore_letto;
                        //stampo la navicella
                        for(i=0; i<3; i++){
                            mvprintw(navicella.y+i, navicella.x, SpriteNavicellaMorta[i]);
                        }
                        break;
                }
                break;

            case Missile:
                mvaddch(missili[valore_letto.id].y, missili[valore_letto.id].x,' ');
                missili[valore_letto.id] = valore_letto;
                mvaddch(missili[valore_letto.id].y, missili[valore_letto.id].x,SpriteMissile);
                for(i=0; i<numNemici; i++){
                    for(n=0; n<2; n++){
                        if(statoNemico[i]==0) {
                            if ((nemico[i].x == missili[n].x && nemico[i].y == missili[n].y) ||
                                (nemico[i].x + 1 == missili[n].x && nemico[i].y == missili[n].y) ||
                                (nemico[i].x + 2 == missili[n].x && nemico[i].y == missili[n].y)
                                || (nemico[i].x == missili[n].x && nemico[i].y + 1 == missili[n].y) ||
                                (nemico[i].x + 1 == missili[n].x && nemico[i].y + 1 == missili[n].y) ||
                                (nemico[i].x + 2 == missili[n].x && nemico[i].y + 1 == missili[n].y)
                                || (nemico[i].x == missili[n].x && nemico[i].y + 2 == missili[n].y) ||
                                (nemico[i].x + 1 == missili[n].x && nemico[i].y + 2 == missili[n].y) ||
                                (nemico[i].x + 2 == missili[n].x && nemico[i].y + 2 == missili[n].y)) {
                                //mvaddch(missili[n].y, missili[n].x,'X');
                                mvaddch(missili[n].y, missili[n].x, ' ');
                                missili[n].x = -1;
                                missili[n].y = -1;
                                kill(missili[n].pid, 1);
                                for (j = 0; j < 3; j++) {
                                    mvprintw(nemico[i].y + j, nemico[i].x, SpriteNemicoMorente[j]);
                                }

                                refresh();
                                usleep(30000);
                                for (j = 0; j < 3; j++) {
                                    mvprintw(nemico[i].y + j, nemico[i].x, "    ");
                                }
                                //nemico[i].x=-4;
                                //nemico[i].y=-4;
                                //kill(nemico[i].pid, 1);

                                statoNemico[i] = 1;
                                //nemiciVivi--;
                            }
                        }
                        else{
                            //primo check enorme
                            if(vitaNemici[i][0]==1 && ((nemico[i].x == missili[n].x && nemico[i].y == missili[n].y) ||
                               (nemico[i].x + 1 == missili[n].x && nemico[i].y == missili[n].y) ||
                               (nemico[i].x + 2 == missili[n].x && nemico[i].y == missili[n].y)
                               || (nemico[i].x == missili[n].x && nemico[i].y + 1 == missili[n].y) ||
                               (nemico[i].x + 1 == missili[n].x && nemico[i].y + 1 == missili[n].y) ||
                               (nemico[i].x + 2 == missili[n].x && nemico[i].y + 1 == missili[n].y)
                               || (nemico[i].x == missili[n].x && nemico[i].y + 2 == missili[n].y) ||
                               (nemico[i].x + 1 == missili[n].x && nemico[i].y + 2 == missili[n].y) ||
                               (nemico[i].x + 2 == missili[n].x && nemico[i].y + 2 == missili[n].y))){
                                for (j = 0; j < 3; j++) {
                                    mvaddch(missili[n].y, missili[n].x, ' ');
                                    missili[n].x = -1;
                                    missili[n].y = -1;
                                    kill(missili[n].pid, 1);
                                    mvprintw(nemico[i].y + j, nemico[i].x, "   ");
                                }
                                vitaNemici[i][0]=0;
                            }
                            //secondo check enorme
                            if(vitaNemici[i][1]==1 && ((nemico[i].x + 4 == missili[n].x && nemico[i].y == missili[n].y) ||
                               (nemico[i].x + 5 == missili[n].x && nemico[i].y == missili[n].y) ||
                               (nemico[i].x + 6 == missili[n].x && nemico[i].y == missili[n].y)
                               || (nemico[i].x + 4== missili[n].x && nemico[i].y + 1 == missili[n].y) ||
                               (nemico[i].x + 5 == missili[n].x && nemico[i].y + 1 == missili[n].y) ||
                               (nemico[i].x + 6 == missili[n].x && nemico[i].y + 1 == missili[n].y)
                               || (nemico[i].x + 4== missili[n].x && nemico[i].y + 2 == missili[n].y) ||
                               (nemico[i].x + 5 == missili[n].x && nemico[i].y + 2 == missili[n].y) ||
                               (nemico[i].x + 6 == missili[n].x && nemico[i].y + 2 == missili[n].y))){
                                for (j = 0; j < 3; j++) {
                                    mvaddch(missili[n].y, missili[n].x, ' ');
                                    missili[n].x = -1;
                                    missili[n].y = -1;
                                    kill(missili[n].pid, 1);
                                    mvprintw(nemico[i].y + j, nemico[i].x + 4, "   ");
                                }
                                vitaNemici[i][1]=0;
                            }

                            if(vitaNemici[i][2]==1 && ((nemico[i].x == missili[n].x && nemico[i].y + 4 == missili[n].y) ||
                               (nemico[i].x + 1 == missili[n].x && nemico[i].y + 4 == missili[n].y) ||
                               (nemico[i].x + 2 == missili[n].x && nemico[i].y + 4== missili[n].y)
                               || (nemico[i].x == missili[n].x && nemico[i].y + 5 == missili[n].y) ||
                               (nemico[i].x + 1 == missili[n].x && nemico[i].y + 5 == missili[n].y) ||
                               (nemico[i].x + 2 == missili[n].x && nemico[i].y + 5 == missili[n].y)
                               || (nemico[i].x == missili[n].x && nemico[i].y + 6 == missili[n].y) ||
                               (nemico[i].x + 1 == missili[n].x && nemico[i].y + 6 == missili[n].y) ||
                               (nemico[i].x + 2 == missili[n].x && nemico[i].y + 6 == missili[n].y))){
                                for (j = 0; j < 3; j++) {
                                    mvaddch(missili[n].y, missili[n].x, ' ');
                                    missili[n].x = -1;
                                    missili[n].y = -1;
                                    kill(missili[n].pid, 1);
                                    mvprintw(nemico[i].y + j + 4, nemico[i].x, "   ");
                                }
                                vitaNemici[i][2]=0;
                            }

                            if(vitaNemici[i][3]==1 && ((nemico[i].x + 4== missili[n].x && nemico[i].y + 4 == missili[n].y) ||
                               (nemico[i].x + 5 == missili[n].x && nemico[i].y + 4 == missili[n].y) ||
                               (nemico[i].x + 6 == missili[n].x && nemico[i].y + 4== missili[n].y)
                               || (nemico[i].x + 4== missili[n].x && nemico[i].y + 5 == missili[n].y) ||
                               (nemico[i].x + 5 == missili[n].x && nemico[i].y + 5 == missili[n].y) ||
                               (nemico[i].x + 6 == missili[n].x && nemico[i].y + 5 == missili[n].y)
                               || (nemico[i].x + 4== missili[n].x && nemico[i].y + 6 == missili[n].y) ||
                               (nemico[i].x + 5 == missili[n].x && nemico[i].y + 6 == missili[n].y) ||
                               (nemico[i].x + 6 == missili[n].x && nemico[i].y + 6 == missili[n].y))){
                                for (j = 0; j < 3; j++) {
                                    mvaddch(missili[n].y, missili[n].x, ' ');
                                    missili[n].x = -1;
                                    missili[n].y = -1;
                                    kill(missili[n].pid, 1);
                                    mvprintw(nemico[i].y + j + 4, nemico[i].x + 4, "   ");
                                }
                                vitaNemici[i][3]=0;
                            }

                            if(vitaNemici[i][0]==0 && vitaNemici[i][1]==0 && vitaNemici[i][2]==0 && vitaNemici[i][3]==0){
                                //mvaddch(missili[n].y, missili[n].x,'X');

                                refresh();
                                usleep(30000);
                                nemico[i].x=-50;
                                nemico[i].y=-50;
                                kill(nemico[i].pid, 1);
                                nemiciVivi--;
                            }

                        }
                    }
                }
                break;
            case Bomba:
                mvaddch(bombe[valore_letto.id].y, bombe[valore_letto.id].x,' ');
                bombe[valore_letto.id] = valore_letto;
                mvaddch(bombe[valore_letto.id].y, bombe[valore_letto.id].x,SpriteBomba);
                //se la bomba tocca la navicella viene persa una vita
                for(i=0; i<numNemici; i++){
                    if((navicella.x==bombe[i].x && navicella.y==bombe[i].y) || (navicella.x+1==bombe[i].x && navicella.y==bombe[i].y) || (navicella.x+2==bombe[i].x && navicella.y==bombe[i].y)
                       || (navicella.x==bombe[i].x && navicella.y+1==bombe[i].y) || (navicella.x+1==bombe[i].x && navicella.y+1==bombe[i].y) || (navicella.x+2==bombe[i].x && navicella.y+1==bombe[i].y) || (navicella.x+3==bombe[i].x && navicella.y+1==bombe[i].y) || (navicella.x+4==bombe[i].x && navicella.y+1==bombe[i].y)
                       || (navicella.x==bombe[i].x && navicella.y+2==bombe[i].y) || (navicella.x+1==bombe[i].x && navicella.y+2==bombe[i].y) || (navicella.x+2==bombe[i].x && navicella.y+2==bombe[i].y)){
                        vite--;
                        //aggiorno le coordinate della bomba che ha toccato la navicella in modo che "scompaia"
                        bombe[i].x=-1;
                        bombe[i].y=-1;
                        //termino il processo che gestisce la bomba
                        kill(bombe[i].pid, 1);

                        int v=i;
                        i=0;
                        switch(vite) {
                            case 2:
                                //elimino la navicella dalle coordinate vecchie
                                for (i = 0; i < 3; i++) {
                                    mvprintw(navicella.y + i, navicella.x, "     ");
                                }
                                //stampo la navicella
                                for (i = 0; i < 3; i++) {
                                    mvprintw(navicella.y + i, navicella.x, SpriteNavicellaDmgd2[i]);
                                }
                                break;
                            case 1:
                                //elimino la navicella dalle coordinate vecchie
                                for (i = 0; i < 3; i++) {
                                    mvprintw(navicella.y + i, navicella.x, "     ");
                                }
                                //stampo la navicella
                                for (i = 0; i < 3; i++) {
                                    mvprintw(navicella.y + i, navicella.x, SpriteNavicellaDmgd3[i]);
                                }
                                break;
                        }
                        i=v;

                    }
                }
                break;
        }
        //stampa vite aggiornate
        mvprintw(0, 1, "Vite: %d", vite);
        for(i=0; i<maxx; i++){
            mvprintw(1, i, "-");
        }
        refresh();
    } while(vite>0 && nemiciVivi>0);
    //stampa messaggio di game over
}

void missile(int pipeout, int maxx, int maxy, int navx, int navy, int diry){

    Position pos_missile;
    pos_missile.x=5+navx;
    pos_missile.y=1+navy;
    pos_missile.i=Missile;
    pos_missile.pid=getpid();
    if(diry==1){
        pos_missile.id=0;
    }
    else{
        pos_missile.id=1;
    }
    int i=0;
    write(pipeout, &pos_missile, sizeof(pos_missile));
    while(!(pos_missile.x>maxx)){
        if(pos_missile.y+diry>maxy || pos_missile.y+diry<2) {diry=-diry;}
        if(i%6==0){
            pos_missile.y+=diry;
        }
        pos_missile.x+=1;
        write(pipeout, &pos_missile, sizeof(pos_missile));
        usleep(10000);
        i++;
    }
    //kill(getpid(),SIGKILL);
    exit(1);

}

void bomba(int pipeout, int x_bomba, int y_bomba, int id){
    Position pos_bomba;
    pos_bomba.x=x_bomba;
    pos_bomba.y=y_bomba;
    pos_bomba.i=Bomba;
    pos_bomba.id=id;
    pos_bomba.pid=getpid();

    write(pipeout, &pos_bomba, sizeof(pos_bomba));

    while(!(pos_bomba.x<0)){
        pos_bomba.x-=1;
        write(pipeout, &pos_bomba, sizeof(pos_bomba));
        usleep(30000);
    }

    exit(1);
}
