#include "controllo.h"

/**
 * Funzione che si occupa della stampa dei vari elementi a schermo e della gestione delle collisioni.
 * @param pipein File descriptor in lettura della pipe.
 * @param maxx Massimo valore delle X sullo schermo.
 * @param maxy Massimo valore delle Y sullo schermo.
 */
void controllo(int pipein, int maxx, int maxy, int numNemici, int valoreDifficolta){

    //dichiarazione delle variabili dove verranno salvate le informazioni lette dalla pipe.
    Position nemico[numNemici], bombe[numNemici], bombeAvanzate[numNemici], navicella, valore_letto, missili[2];
    int i, punti=0, cicli=1, vite=3, n, j;
    //array che serve per tenere traccia dello stato deli nemici, ovvero se sono di primo (valore 0) o di secondo (valore 1) livello.
    int statoNemico[numNemici];
    //matrice per tenere traccia quali nemici di secondo livello sono vivi.
    int vitaNemici[numNemici][4];
    //inizializzazione delle variabili dichiarate sopra.
    navicella.x=-1;
    for(i=0; i<numNemici; i++){
        statoNemico[i]=0;
        nemico[i].x=-1;
        bombe[i].x=-1;
        bombeAvanzate[i].x=-1;
        for(j=0;j<4;j++){
            vitaNemici[i][j]=1;
        }
    }
    int nemiciVivi=numNemici, nemiciSecLiv=0;
    //stampa delle informazioni iniziali
    mvprintw(0, 1, "Vite: %d", vite);
    mvprintw(0, 10, "Nemici: %d  ", nemiciVivi);
    mvprintw(0, 40, "Nemici secondo livello: %d  ", nemiciSecLiv);
    mvprintw(0, maxx-15, "Punti: %d   ", punti);
    for(i=0; i<maxx; i++){
        mvprintw(1, i, "-");
    }
    do{
        //leggo un valore dalla pipe.
        read(pipein, &valore_letto, sizeof(valore_letto));

        //controllo che tipo di valore ho letto.
        switch (valore_letto.i) {
            case Nemico:
                //cambio del colore per la stampa del nemico.
                attron(COLOR_PAIR(2));
                //controllo se il nemico letto dalla pipe è di primo o di secondo livello
                switch(statoNemico[valore_letto.id]){
                    case 0: //nemico di primo livello.
                        //elimino il nemico dalle coordinate vecchie.
                        for (i = 0; i < 3; i++) {
                            mvprintw(nemico[valore_letto.id].y + i, nemico[valore_letto.id].x, "    ");
                        }
                        //aggiorno le coordinate del nemico.
                        nemico[valore_letto.id] = valore_letto;
                        //stampo il nemico.
                        for (i = 0; i < 3; i++) {
                            mvprintw(nemico[valore_letto.id].y + i, nemico[valore_letto.id].x, SpriteNemicoBase[i]);
                        }
                        break;
                    //nemico di secondo livello.
                    case 1:
                        //elimino la stampa delle 4 navicelle (di quelle che sono ancora vive) dalle coordinate vecchie
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
                        //aggiorno le coordinate del nemico.
                        nemico[valore_letto.id] = valore_letto;
                        mvprintw(nemico[valore_letto.id].y+3, nemico[valore_letto.id].x+3, "X");

                        //stampo il nemico.
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
                //imposto il colore di base.
                attron(COLOR_PAIR(1));
                //controllo se il nemico letto ha raggiunto la navicella (condizione per perdere).
                if(nemico[valore_letto.id].x<2){
                    //imposto le vite della navicella a zero.
                    vite=0;
                }
                break;
            case Navicella:
                //cambio del colore per la stampa della navicella.
                attron(COLOR_PAIR(3));
                //in base alle vite rimesta della navicella stampo uno strite diverso (der dare un senso di danneggiamento della navicella).
                switch(vite){
                    case 3:
                        //elimino la navicella dalle coordinate vecchie.
                        for (i = 0; i < 3; i++) {
                            mvprintw(navicella.y + i, navicella.x, "     ");
                        }
                        //aggiorno le coordinate della navicella.
                        navicella = valore_letto;
                        //stampo la navicella.
                        for(i=0; i<3; i++){
                            mvprintw(navicella.y+i, navicella.x, SpriteNavicella[i]);
                        }
                        break;
                    case 2:
                        //elimino la navicella dalle coordinate vecchie.
                        for (i = 0; i < 3; i++) {
                            mvprintw(navicella.y + i, navicella.x, "     ");
                        }
                        //aggiorno le coordinate della navicella.
                        navicella = valore_letto;
                        //stampo la navicella.
                        for(i=0; i<3; i++){
                            mvprintw(navicella.y+i, navicella.x, SpriteNavicellaDmgd2[i]);
                        }
                        break;
                    case 1:
                        //elimino la navicella dalle coordinate vecchie.
                        for (i = 0; i < 3; i++) {
                            mvprintw(navicella.y + i, navicella.x, "     ");
                        }
                        //aggiorno le coordinate della navicella.
                        navicella = valore_letto;
                        //stampo la navicella.
                        for(i=0; i<3; i++){
                            mvprintw(navicella.y+i, navicella.x, SpriteNavicellaDmgd3[i]);
                        }
                        break;
                    case 0:
                        //elimino la navicella dalle coordinate vecchie.
                        for (i = 0; i < 3; i++) {
                            mvprintw(navicella.y + i, navicella.x, "     ");
                        }
                        //aggiorno le coordinate della navicella.
                        navicella = valore_letto;
                        //stampo la navicella.
                        for(i=0; i<3; i++){
                            mvprintw(navicella.y+i, navicella.x, SpriteNavicellaMorta[i]);
                        }
                        break;
                }
                //imposto il colore di base.
                attron(COLOR_PAIR(1));
                break;

            case Missile:
                //elimino il missile dalle coordinate vecchie.
                mvaddch(missili[valore_letto.id].y, missili[valore_letto.id].x,' ');
                //aggiorno le coordinate del missile.
                missili[valore_letto.id] = valore_letto;
                //stampo il missile nella nuova posizione.
                mvaddch(missili[valore_letto.id].y, missili[valore_letto.id].x,SpriteMissile);
                //controllo delle collisioni del missile
                for(i=0; i<numNemici; i++){
                    for(n=0; n<2; n++){
                        //collisione missie-bomba.
                        if(bombe[i].x == missili[n].x && bombe[i].y == missili[n].y){
                            //elimino dallo schermo il missile e la bomba.
                            mvaddch(missili[n].y, missili[n].x, ' ');
                            //termino il processo che gestiva il missile.
                            kill(missili[n].pid, SIGKILL);
                            //termino il processo che gestiva la bomba.
                            kill(bombe[i].pid, 1);
                            //imposto le coordnate del missile e della bomba fuori dallo schermo.
                            missili[n].x = -30;
                            missili[n].y = -30;
                            bombe[i].x = -1;
                            bombe[i].y = -1;
                        }
                        //nel caso ci siano delle bombe generate da navicelle di secondo livello controllo anche quelle collisioni.
                        if(statoNemico[i]==1){
                            if(bombeAvanzate[i].x == missili[n].x && bombeAvanzate[i].y == missili[n].y){
                                //elimino dallo schermo il missile e la bomba.
                                mvaddch(missili[n].y, missili[n].x, ' ');
                                //termino il processo che gestiva il missile.
                                kill(missili[n].pid, SIGKILL);
                                //termino il processo che gestiva la bomba.
                                kill(bombeAvanzate[i].pid, 1);
                                //imposto le coordnate del missile e della bomba fuori dallo schermo.
                                missili[n].x = -30;
                                missili[n].y = -30;
                                bombeAvanzate[i].x = -1;
                                bombeAvanzate[i].y = -1;
                            }
                        }
                        //collisione missile-nemico di primo livello.
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
                                //aggiorno il numero di nemici presenti a schermo.
                                nemiciVivi+=3;
                                nemiciSecLiv+=4;
                                //elimino dallo schermo il missile.
                                mvaddch(missili[n].y, missili[n].x, ' ');
                                //termino il processo che gestiva il missile.
                                kill(missili[n].pid, SIGKILL);
                                //imposto le coordnate del missile dallo schermo.
                                missili[n].x = -30;
                                missili[n].y = -30;
                                //imposto il colore per la stampa del nemico.
                                attron(COLOR_PAIR(2));
                                //stampa dello sprite del nemico morente.
                                for (j = 0; j < 3; j++) {
                                    mvprintw(nemico[i].y + j, nemico[i].x, SpriteNemicoMorente[j]);
                                }
                                //aggiungo i punti dati dal nemico colpito in base alla difficoltà.
                                switch(valoreDifficolta){
                                    case 0:
                                        punti+=200;
                                        break;
                                    case 1:
                                        punti+=400;
                                        break;
                                    case 2:
                                        punti+=500;
                                        break;
                                }
                                //aggiorno lo schermo.
                                refresh();
                                //imposto un delay per far vedere il nemico morente.
                                usleep(30000);
                                //imposto il colore di base.
                                attron(COLOR_PAIR(1));
                                //modifico lo stato del nemico colpito da primo a secondo livello.
                                statoNemico[i] = 1;
                            }
                        }
                        //collisione missile-nemico di secondo livello.
                        else if(statoNemico[i]==1){
                            //collisione missile-nemico di secondo livello in alto a sinistra.
                            if(vitaNemici[i][0]==1 && ((nemico[i].x == missili[n].x && nemico[i].y == missili[n].y) ||
                                                       (nemico[i].x + 1 == missili[n].x && nemico[i].y == missili[n].y) ||
                                                       (nemico[i].x + 2 == missili[n].x && nemico[i].y == missili[n].y)
                                                       || (nemico[i].x == missili[n].x && nemico[i].y + 1 == missili[n].y) ||
                                                       (nemico[i].x + 1 == missili[n].x && nemico[i].y + 1 == missili[n].y) ||
                                                       (nemico[i].x + 2 == missili[n].x && nemico[i].y + 1 == missili[n].y)
                                                       || (nemico[i].x == missili[n].x && nemico[i].y + 2 == missili[n].y) ||
                                                       (nemico[i].x + 1 == missili[n].x && nemico[i].y + 2 == missili[n].y) ||
                                                       (nemico[i].x + 2 == missili[n].x && nemico[i].y + 2 == missili[n].y))){
                                //elimino dallo schermo il missile.
                                mvaddch(missili[n].y, missili[n].x, ' ');
                                //termino il processo che gestiva il missile.
                                kill(missili[n].pid, SIGKILL);
                                //imposto le coordnate del missile dallo schermo.
                                missili[n].x = -30;
                                missili[n].y = -30;
                                //elimino dallo schermo il nemico di secondo livello colpito.
                                for (j = 0; j < 3; j++) {
                                    mvprintw(nemico[i].y + j, nemico[i].x, "   ");
                                }
                                //imposto il nemico di secondo livello colpito come morto.
                                vitaNemici[i][0]=0;
                                //diminuisco il numero di nemici vivi.
                                nemiciVivi--;
                                nemiciSecLiv--;
                                //aggiungo i punti dati dal nemico colpito in base alla difficoltà.
                                switch(valoreDifficolta){
                                    case 0:
                                        punti+=70;
                                        break;
                                    case 1:
                                        punti+=150;
                                        break;
                                    case 2:
                                        punti+=250;
                                        break;
                                }
                            }
                            //collisione missile-nemico di secondo livello in alto a destra.
                            if(vitaNemici[i][1]==1 && ((nemico[i].x + 4 == missili[n].x && nemico[i].y == missili[n].y) ||
                                                       (nemico[i].x + 5 == missili[n].x && nemico[i].y == missili[n].y) ||
                                                       (nemico[i].x + 6 == missili[n].x && nemico[i].y == missili[n].y)
                                                       || (nemico[i].x + 4== missili[n].x && nemico[i].y + 1 == missili[n].y) ||
                                                       (nemico[i].x + 5 == missili[n].x && nemico[i].y + 1 == missili[n].y) ||
                                                       (nemico[i].x + 6 == missili[n].x && nemico[i].y + 1 == missili[n].y)
                                                       || (nemico[i].x + 4== missili[n].x && nemico[i].y + 2 == missili[n].y) ||
                                                       (nemico[i].x + 5 == missili[n].x && nemico[i].y + 2 == missili[n].y) ||
                                                       (nemico[i].x + 6 == missili[n].x && nemico[i].y + 2 == missili[n].y))){
                                //elimino dallo schermo il missile.
                                mvaddch(missili[n].y, missili[n].x, ' ');
                                //termino il processo che gestiva il missile.
                                kill(missili[n].pid, SIGKILL);
                                //imposto le coordnate del missile dallo schermo.
                                missili[n].x = -30;
                                missili[n].y = -30;
                                //elimino dallo schermo il nemico di secondo livello colpito.
                                for (j = 0; j < 3; j++) {
                                    mvprintw(nemico[i].y + j, nemico[i].x + 4, "   ");
                                }
                                //imposto il nemico di secondo livello colpito come morto.
                                vitaNemici[i][1]=0;
                                //diminuisco il numero di nemici vivi.
                                nemiciVivi--;
                                nemiciSecLiv--;
                                //aggiungo i punti dati dal nemico colpito in base alla difficoltà.
                                switch(valoreDifficolta){
                                    case 0:
                                        punti+=70;
                                        break;
                                    case 1:
                                        punti+=150;
                                        break;
                                    case 2:
                                        punti+=250;
                                        break;
                                }
                            }
                            //collisione missile-nemico di secondo livello in basso a sinistra.
                            if(vitaNemici[i][2]==1 && ((nemico[i].x == missili[n].x && nemico[i].y + 4 == missili[n].y) ||
                                                       (nemico[i].x + 1 == missili[n].x && nemico[i].y + 4 == missili[n].y) ||
                                                       (nemico[i].x + 2 == missili[n].x && nemico[i].y + 4== missili[n].y)
                                                       || (nemico[i].x == missili[n].x && nemico[i].y + 5 == missili[n].y) ||
                                                       (nemico[i].x + 1 == missili[n].x && nemico[i].y + 5 == missili[n].y) ||
                                                       (nemico[i].x + 2 == missili[n].x && nemico[i].y + 5 == missili[n].y)
                                                       || (nemico[i].x == missili[n].x && nemico[i].y + 6 == missili[n].y) ||
                                                       (nemico[i].x + 1 == missili[n].x && nemico[i].y + 6 == missili[n].y) ||
                                                       (nemico[i].x + 2 == missili[n].x && nemico[i].y + 6 == missili[n].y))){
                                //elimino dallo schermo il missile.
                                mvaddch(missili[n].y, missili[n].x, ' ');
                                //termino il processo che gestiva il missile.
                                kill(missili[n].pid, SIGKILL);
                                //imposto le coordnate del missile dallo schermo.
                                missili[n].x = -30;
                                missili[n].y = -30;
                                //elimino dallo schermo il nemico di secondo livello colpito.
                                for (j = 0; j < 3; j++) {
                                    mvprintw(nemico[i].y + j + 4, nemico[i].x, "   ");
                                }
                                //imposto il nemico di secondo livello colpito come morto.
                                vitaNemici[i][2]=0;
                                //diminuisco il numero di nemici vivi.
                                nemiciVivi--;
                                nemiciSecLiv--;
                                //aggiungo i punti dati dal nemico colpito in base alla difficoltà.
                                switch(valoreDifficolta){
                                    case 0:
                                        punti+=70;
                                        break;
                                    case 1:
                                        punti+=150;
                                        break;
                                    case 2:
                                        punti+=250;
                                        break;
                                }
                            }
                            //collisione missile-nemico di secondo livello in basso a destra.
                            if(vitaNemici[i][3]==1 && ((nemico[i].x + 4== missili[n].x && nemico[i].y + 4 == missili[n].y) ||
                                                       (nemico[i].x + 5 == missili[n].x && nemico[i].y + 4 == missili[n].y) ||
                                                       (nemico[i].x + 6 == missili[n].x && nemico[i].y + 4== missili[n].y)
                                                       || (nemico[i].x + 4== missili[n].x && nemico[i].y + 5 == missili[n].y) ||
                                                       (nemico[i].x + 5 == missili[n].x && nemico[i].y + 5 == missili[n].y) ||
                                                       (nemico[i].x + 6 == missili[n].x && nemico[i].y + 5 == missili[n].y)
                                                       || (nemico[i].x + 4== missili[n].x && nemico[i].y + 6 == missili[n].y) ||
                                                       (nemico[i].x + 5 == missili[n].x && nemico[i].y + 6 == missili[n].y) ||
                                                       (nemico[i].x + 6 == missili[n].x && nemico[i].y + 6 == missili[n].y))){
                                //elimino dallo schermo il missile.
                                mvaddch(missili[n].y, missili[n].x, ' ');
                                //termino il processo che gestiva il missile.
                                kill(missili[n].pid, SIGKILL);
                                //imposto le coordnate del missile dallo schermo.
                                missili[n].x = -30;
                                missili[n].y = -30;
                                //elimino dallo schermo il nemico di secondo livello colpito.
                                for (j = 0; j < 3; j++) {
                                    mvprintw(nemico[i].y + j + 4, nemico[i].x + 4, "   ");
                                }
                                //imposto il nemico di secondo livello colpito come morto.
                                vitaNemici[i][3]=0;
                                //diminuisco il numero di nemici vivi.
                                nemiciVivi--;
                                nemiciSecLiv--;
                                //aggiungo i punti dati dal nemico colpito in base alla difficoltà.
                                switch(valoreDifficolta){
                                    case 0:
                                        punti+=70;
                                        break;
                                    case 1:
                                        punti+=150;
                                        break;
                                    case 2:
                                        punti+=250;
                                        break;
                                }
                            }
                            //se il blocco di nemici di secondo livello gestiti da un processo viene distrutto.
                            if(vitaNemici[i][0]==0 && vitaNemici[i][1]==0 && vitaNemici[i][2]==0 && vitaNemici[i][3]==0){
                                mvprintw(nemico[i].y+3, nemico[i].x+3, " ");
                                mvprintw(bombe[i].y, bombe[i].x, " ");
                                mvprintw(bombeAvanzate[i].y, bombeAvanzate[i].x, " ");
                                //termino il processo che gestiva i nemici e delle bombe generate da essi.
                                kill(nemico[i].pid, 1);
                                kill(bombe[i].pid, 1);
                                kill(bombeAvanzate[i].pid, 1);
                                //imposto le coordinate dei nemici e delle bombe fuori dallo schermo.
                                nemico[i].x=-10;
                                nemico[i].y=-10;
                                bombe[i].x=-1;
                                bombe[i].y=-1;
                                bombeAvanzate[i].x=-1;
                                bombeAvanzate[i].y=-1;
                                //aggiorno lo schermo.
                                refresh();
                                //modifico lo stato del nemico colpito da secondo livello a distrutto.
                                statoNemico[i]=3;
                            }
                        }
                    }
                }
                break;
            case Bomba:
                //cambio del colore per la stampa della bomba.
                attron(COLOR_PAIR(2));
                //cancello la bomba dalle coordinate vecchie.
                mvaddch(bombe[valore_letto.id].y, bombe[valore_letto.id].x,' ');
                //aggiorno le coordinate della bomba.
                bombe[valore_letto.id] = valore_letto;
                //stampo la bomba nella nuova posizione.
                mvaddch(bombe[valore_letto.id].y, bombe[valore_letto.id].x,SpriteBomba);
                //collisione bomba-navicella
                for(i=0; i<numNemici; i++){
                    if((navicella.x==bombe[i].x && navicella.y==bombe[i].y) || (navicella.x+1==bombe[i].x && navicella.y==bombe[i].y) || (navicella.x+2==bombe[i].x && navicella.y==bombe[i].y)
                       || (navicella.x==bombe[i].x && navicella.y+1==bombe[i].y) || (navicella.x+1==bombe[i].x && navicella.y+1==bombe[i].y) || (navicella.x+2==bombe[i].x && navicella.y+1==bombe[i].y) || (navicella.x+3==bombe[i].x && navicella.y+1==bombe[i].y) || (navicella.x+4==bombe[i].x && navicella.y+1==bombe[i].y)
                       || (navicella.x==bombe[i].x && navicella.y+2==bombe[i].y) || (navicella.x+1==bombe[i].x && navicella.y+2==bombe[i].y) || (navicella.x+2==bombe[i].x && navicella.y+2==bombe[i].y)){
                        //la navicella quando colpita da una bomba perde una vita.
                        vite--;
                        //aggiorno le coordinate della bomba che ha toccato la navicella fuori dallo schermo.
                        bombe[i].x=-1;
                        bombe[i].y=-1;
                        //termino il processo che gestisce la bomba.
                        kill(bombe[i].pid, 1);
                    }
                }
                //in base alle vite rimaste ambio lo sprite della navicella.
                attron(COLOR_PAIR(3));
                switch(vite) {
                    case 2:
                        for (i = 0; i < 3; i++) {
                            mvprintw(navicella.y + i, navicella.x, "     ");
                        }
                        for (i = 0; i < 3; i++) {
                            mvprintw(navicella.y + i, navicella.x, SpriteNavicellaDmgd2[i]);
                        }
                        break;
                    case 1:
                        for (i = 0; i < 3; i++) {
                            mvprintw(navicella.y + i, navicella.x, "     ");
                        }
                        for (i = 0; i < 3; i++) {
                            mvprintw(navicella.y + i, navicella.x, SpriteNavicellaDmgd3[i]);
                        }
                        break;
                }
                attron(COLOR_PAIR(1));
                break;
            case BombaAvanzata:
                if(statoNemico[valore_letto.id]==1){
                    //cambio del colore per la stampa della bomba.
                    attron(COLOR_PAIR(2));
                    //cancello la bomba dalle coordinate vecchie.
                    mvaddch(bombeAvanzate[valore_letto.id].y, bombeAvanzate[valore_letto.id].x,' ');
                    //aggiorno le coordinate della bomba.
                    bombeAvanzate[valore_letto.id] = valore_letto;
                    //stampo la bomba nella nuova posizione.
                    mvaddch(bombeAvanzate[valore_letto.id].y, bombeAvanzate[valore_letto.id].x,SpriteBomba);
                    //collisione bomba-navicella
                    for(i=0; i<numNemici; i++){
                        if((navicella.x==bombeAvanzate[i].x && navicella.y==bombeAvanzate[i].y) || (navicella.x+1==bombeAvanzate[i].x && navicella.y==bombeAvanzate[i].y) || (navicella.x+2==bombeAvanzate[i].x && navicella.y==bombeAvanzate[i].y)
                           || (navicella.x==bombeAvanzate[i].x && navicella.y+1==bombeAvanzate[i].y) || (navicella.x+1==bombeAvanzate[i].x && navicella.y+1==bombeAvanzate[i].y) || (navicella.x+2==bombeAvanzate[i].x && navicella.y+1==bombeAvanzate[i].y) || (navicella.x+3==bombeAvanzate[i].x && navicella.y+1==bombeAvanzate[i].y) || (navicella.x+4==bombeAvanzate[i].x && navicella.y+1==bombeAvanzate[i].y)
                           || (navicella.x==bombeAvanzate[i].x && navicella.y+2==bombeAvanzate[i].y) || (navicella.x+1==bombeAvanzate[i].x && navicella.y+2==bombeAvanzate[i].y) || (navicella.x+2==bombeAvanzate[i].x && navicella.y+2==bombeAvanzate[i].y)){
                            //la navicella quando colpita da una bomba perde una vita.
                            vite--;
                            //aggiorno le coordinate della bomba che ha toccato la navicella fuori dallo schermo.
                            bombeAvanzate[i].x=-1;
                            bombeAvanzate[i].y=-1;
                            //termino il processo che gestisce la bomba.
                            kill(bombeAvanzate[i].pid, 1);
                        }
                    }
                    //in base alle vite rimaste ambio lo sprite della navicella.
                    attron(COLOR_PAIR(3));
                    switch(vite) {
                        case 2:
                            for (i = 0; i < 3; i++) {
                                mvprintw(navicella.y + i, navicella.x, "     ");
                            }
                            for (i = 0; i < 3; i++) {
                                mvprintw(navicella.y + i, navicella.x, SpriteNavicellaDmgd2[i]);
                            }
                            break;
                        case 1:
                            for (i = 0; i < 3; i++) {
                                mvprintw(navicella.y + i, navicella.x, "     ");
                            }
                            for (i = 0; i < 3; i++) {
                                mvprintw(navicella.y + i, navicella.x, SpriteNavicellaDmgd3[i]);
                            }
                            break;
                    }
                    attron(COLOR_PAIR(1));
                }
                break;
        }
        //ogni tot cicli i punti vengono decrementati.
        if(!(cicli++%500)){
            if(punti>0){
                punti--;
            }
        }
        //stampa delle informazioni aggiornate.
        mvprintw(0, 1, "Vite: %d", vite);
        mvprintw(0, 10, "Nemici: %d  ", nemiciVivi);
        mvprintw(0, 40, "Nemici secondo livello: %d  ", nemiciSecLiv);
        mvprintw(0, maxx-15, "Punti: %d   ", punti);
        for(i=0; i<maxx; i++){
            mvprintw(1, i, "-");
        }
        //aggiornamento degli elementi a schermo.
        refresh();
    } while(vite>0 && nemiciVivi>0); //ciclo termina quando la navicella non ha più vite oppure quando tutti i nemici sono stati distrutti.

    //pulisco lo schermo.
    clear();
    int uscita=0;
    //stampa di game over quando si perde.
    if(vite==0){
        attron(COLOR_PAIR(3));
        for(i=0; i<7; i++){
            mvprintw(maxy/2-10+i, maxx/2-50, gameover[i]);
        }
        attron(COLOR_PAIR(1));
        while(uscita!=32){
            mvprintw(maxy/2-2, maxx/2-13,"Hai totalizzato %d punti", punti);
            mvprintw(maxy/2, maxx/2-12,"Premi spazio per uscire");
            timeout(100);
            uscita=getch();
            refresh();
        }

    }
    //stampa di game over quando si vince.
    else if(nemiciVivi==0 && valoreDifficolta!=3){
        attron(COLOR_PAIR(3));
        for(i=0; i<7; i++){
            mvprintw(maxy/2-10+i, maxx/2-50, youwon[i]);
        }
        attron(COLOR_PAIR(1));
        while(uscita!=32){
            mvprintw(maxy/2-2, maxx/2-13,"Hai totalizzato %d punti", punti);
            mvprintw(maxy/2, maxx/2-12,"Premi spazio per uscire");
            timeout(100);
            uscita=getch();
            refresh();
        }
    }

}

/**
 * Funzione che si occupa del menù iniziale del gioco.
 * @param maxx Massimo valore delle X sullo schermo.
 * @param maxy Massimo valore delle Y sullo schermo.
 * @return Numero che identifica la selezione della difficoltà.
 */
int menu(int maxx, int maxy){
    //nel caso in cui lo schermo sia troppo piccolo viene visualizzato questo messaggio
    if(maxx<140 || maxy<20){
        while(1){
            mvprintw(1,1,"Risoluzione troppo bassa");
            mvprintw(2,1,"Risoluzione minima: 140(x) caratteri per 20(y) caratteri");
            mvprintw(3,1,"Ridimensiona e riesegui :(");
            refresh();
        }
    }
    int isAnimationDone=0, scelta, numScelta=0, i, j=0;
    while(1){
        //stampa della linea superiore
        attron(COLOR_PAIR(1));
        mvprintw(0, 1, "SpaceDefenders");
        mvprintw(0, (maxx/2)-33, "SpaceDefenders      Andrea Martis / Alessio Largiu      Unica 2021");

        for(i=0; i<maxx; i++){
            mvprintw(1, i, "-");
        }

        //stampa del logo
        if(isAnimationDone==0){

            usleep(10000);
            for(i=8; i<15; i++){
                attron(COLOR_PAIR(2));
                for (j = 0; j < 8; j++) {
                    mvprintw(i+j, (maxx/2)-63, logo[j]);
                }
                for (j = 0; j < 8; j++) {
                    mvprintw(i+j-6,  (maxx/2)-63, "                                                                                                                               ");
                }
                usleep(80000);
                refresh();
            }
            attron(COLOR_PAIR(1));
        }

        attron(COLOR_PAIR(3));
        for (j = 0; j < 8; j++) {
            mvprintw(i+j-1,  (maxx/2)-63, logo[j]);
        }

        //stampa della prima banda orizzontale
        attron(COLOR_PAIR(2));
        for(i=0; i<maxx; i++){
            mvprintw(22, i, "+");
            mvprintw(23, i, "+");
            if(isAnimationDone==0){
                usleep(10000);
                refresh();
            }
        }
        attron(COLOR_PAIR(1));

        //stampa selezione della difficoltà
        if(isAnimationDone==1){
            mvprintw(25, (maxx/2)-16, "Seleziona la modalita' di gioco");

            mvprintw(27, (maxx/2)-4, "Facile");
            mvprintw(28, (maxx/2)-4, "Medio");
            mvprintw(29, (maxx/2)-4, "Difficile");
            mvprintw(30, (maxx/2)-4, "Esci");

            switch(scelta){
                case KEY_UP:
                    numScelta--;
                    if(numScelta<0){
                        numScelta=3;
                    }
                    break;

                case KEY_DOWN:
                    numScelta++;

                    if(numScelta>3){
                        numScelta=0;
                    }
                    break;

                case 10:
                    return numScelta;
                    break;
            }

            //stampa del cursore
            attron(COLOR_PAIR(3));
            //int miStoComplicandoLaVita=0;
            for(j=0; j<=3; j++){
                if(j==numScelta){
                    mvprintw(27+j, (maxx/2)-6, ">");
                } else {
                    mvprintw(27+j, (maxx/2)-6, " ");
                }
            }
            attron(COLOR_PAIR(1));

        }

        //stampa della seconda banda orizzontale
        attron(COLOR_PAIR(3));
        for(i=maxx; i>=0; i--){
            mvprintw(32, i, "+");
            mvprintw(33, i, "+");
            if(isAnimationDone==0){
                usleep(10000);
                refresh();
            }
        }
        attron(COLOR_PAIR(1));

        //animazione della navicella e del nemico
        if(isAnimationDone==0){
            for (i = 0; i < 3; i++) {
                mvprintw(4 + i, 10, SpriteNavicella[i]);
            }

            for (i = 0; i < 3; i++) {
                mvprintw(4 + i, 132, SpriteNemicoBase[i]);
            }

            for(i=0;i<=117;i++){
                mvprintw(5, 16+i, ">");
                usleep(20000);
                mvprintw(5, 16+i-1, " ");
                refresh();

            }

            mvprintw(5,133," ");

            for (i = 0; i < 3; i++) {
                mvprintw(4 + i, 132, SpriteNemicoMorente[i]);
            }
            refresh();
            usleep(200000);
            for (i = 0; i < 3; i++) {
                mvprintw(4 + i, 132, "    ");
            }
            refresh();
            for(j=0; j<(maxx/2);j++){
                for (i = 0; i < 3; i++) {
                    mvprintw(4 + i, j+10, SpriteNavicella[i]);
                }
                for (i = 0; i < 3; i++) {
                    mvprintw(4 + i, 2-j+10, "    ");
                }
                refresh();
                usleep(10000);
            }
        }

        if(isAnimationDone==1){
            for (i = 0; i < 3; i++) {
                mvprintw(4 + i, maxx/2+10, SpriteNavicella[i]);
            }

        }

        timeout(100);
        scelta=getch();

        usleep(10000);

        refresh();
        isAnimationDone=1;
    }
}