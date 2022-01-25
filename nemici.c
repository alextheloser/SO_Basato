#include "nemici.h"

/**
 * Funzione che si occupa di generare le coordinate di una navicella nemica.
 * @param pipeout File descriptor in scrittura della pipe.
 * @param x Coordinata X iniziale del nemico.
 * @param y Coordinata Y iniziale del nemico.
 * @param idNemico Numero identificativo della navicella nemica.
 * @param maxx Massimo valore delle X sullo schermo.
 * @param maxy Massimo valore delle Y sullo schermo.
 */
void nemici(int pipeout, int x, int y, int idNemico, int maxx, int maxy){

    Position pos_nemico;
    //assegno le informazioni del nemico alla variabile pos_nemico.
    pos_nemico.x=x;
    pos_nemico.y=y;
    pos_nemico.i=Nemico;
    pos_nemico.id=idNemico;
    pos_nemico.pid=getpid();
    pid_t pid_bomba1, pid_bomba2;
    int r=1, dirx, diry, cicli=1, sigBomba1, sigBomba2;

    //scrivo nella pipe le informazioni iniziali.
    write(pipeout, &pos_nemico, sizeof(pos_nemico));
    while(1){
        //effettuo il movimento del nemico.
        dirx=-PASSO;
        pos_nemico.x+=dirx;
        //alterna il movimento verticale andando in basso e in alto.
        if(r%2==0)
            diry=PASSO;
        else
            diry=-PASSO;
        pos_nemico.y+=diry;
        r++;

        //ogni 5 cicli vengono generate le bombe.
        if(!(cicli++%5)){
            //creazione processo della prima bomba (quella generata dal nemico di primo livello e una delle due generate dal nemico di secondo livello).
            pid_bomba1=fork();
            switch(pid_bomba1){
                //gestione dell'errore.
                case -1:
                    perror("Errore nell'esecuzione della fork!");
                    exit(1);
                //prima bomba.
                case 0:
                    bomba(pipeout, pos_nemico.x-1, pos_nemico.y+1, idNemico, 0);
                default:
                    //creazione processo della seconda bomba, che apparirà quando il nemico diventa di secondo livello.
                    pid_bomba2=fork();
                    switch(pid_bomba2){
                        //gestione dell'errore.
                        case -1:
                            perror("Errore nell'esecuzione della fork!");
                            exit(1);
                        //seconda bomba.
                        case 0:
                            bomba(pipeout, pos_nemico.x-1, pos_nemico.y+5, idNemico, 1);
                        default:
                            break;
                    }
            }
        }
        //aspetto la terminazione delle bombe.
        wait(&sigBomba1);
        wait(&sigBomba2);
        //scrivo nella pipe le informazioni aggiornate.
        write(pipeout,&pos_nemico,sizeof(pos_nemico));
        //delay dello spostamento del nemico.
        usleep(DELAY_NEMICI);
    }
}

/**
 * Funzione che si occupa di generare le coordinate di una bomba.
 * @param pipeout File descriptor in scrittura della pipe.
 * @param x_bomba Coordinata X iniziale della bomba.
 * @param y_bomba Coordinata Y iniziale della bomba.
 * @param id Numero identificativo della bomba (stesso del nemico che la spara)
 * @param type variabile per definire se la bomba è sparata da un nemico di primo livello o da un nemico di secondo livello
 */
void bomba(int pipeout, int x_bomba, int y_bomba, int id, int type){

    Position pos_bomba;
    //assegno le informazioni della bomba alla variabile pos_bomba.
    pos_bomba.x=x_bomba;
    pos_bomba.y=y_bomba;
    if(type==0){
        pos_bomba.i=Bomba;
    }
    else{
        pos_bomba.i=BombaAvanzata;
    }
    pos_bomba.id=id;
    pos_bomba.pid=getpid();

    //scrivo nella pipe le informazioni iniziali.
    write(pipeout, &pos_bomba, sizeof(pos_bomba));

    //la bomba continua a muoversi fino a che non raggiunge il limite sinistro dello schermo.
    while(!(pos_bomba.x<0)){
        //effettuo lo spostamento della bomba.
        pos_bomba.x-=PASSO;
        //scrivo nella pipe le informazioni aggiornate.
        write(pipeout, &pos_bomba, sizeof(pos_bomba));
        //delay dello spostamento della bomba.
        usleep(DELAY_BOMBA);
    }
    //il processo viene terminato quando esce dal while.
    exit(1);
}