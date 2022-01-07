#include "navicella.h"

/**
 * Funzione che si occupa di generare le coordinate della navicella.
 * @param pipeout File descriptor in scrittura della pipe.
 * @param maxx Massimo valore delle X sullo schermo.
 * @param maxy Massimo valore delle Y sullo schermo.
 */
void navicella(int pipeout, int maxx, int maxy){

    Position pos_navicella;
    //assegno le informazioni della navicella alla variabile pos_navicella.
    pos_navicella.x=2;
    pos_navicella.y=maxy/2;
    pos_navicella.i=Navicella;
    pid_t pid_missile1, pid_missile2;
    int isMissileVivo1=0, isMissileVivo2=0, sig1, sig2, c;

    //scrivo nella pipe le informazioni iniziali.
    write(pipeout, &pos_navicella, sizeof(pos_navicella));

    while(1) {
        //acquisisco un carattere dalla tastiera.
        c=getch();
        switch(c) {
            //movimento verso l'alto.
            case KEY_UP:
                if (pos_navicella.y > 2) {
                    pos_navicella.y--;
                }
                break;
            //movimento verso il basso.
            case KEY_DOWN:
                if (pos_navicella.y < maxy - 3) {
                    pos_navicella.y++;
                }
                break;
            //quando viene premuto spazio vengono generati i missili.
            case ' ':
                //i missili vengono sparati solo se non ci sono altri missili sullo schermo.
                if ((isMissileVivo1==0 || isMissileVivo2==0) || (waitpid(pid_missile1,&sig1,WNOHANG)==pid_missile1 && waitpid(pid_missile2,&sig2,WNOHANG)==pid_missile2)){
                    isMissileVivo1=1;
                    isMissileVivo2=1;
                    //creo il processo del primo missile.
                    pid_missile1 = fork();
                    switch (pid_missile1) {
                        //gestione dell'errore.
                        case -1:
                            perror("Errore nell'esecuzione della fork!");
                            exit(1);
                        //primo missile.
                        case 0:
                            missile(pipeout, maxx, maxy, pos_navicella.x, pos_navicella.y, PASSO);
                        default:
                            //creo il processo del secondo missile.
                            pid_missile2=fork();
                            switch(pid_missile2){
                                //gestione dell'errore.
                                case -1:
                                    perror("Errore nell'esecuzione della fork!");
                                    exit(1);
                                //secondo missile.
                                case 0:
                                    missile(pipeout, maxx, maxy, pos_navicella.x, pos_navicella.y, -PASSO);
                                default:
                                    break;
                            }
                    }
                }
                break;
        }
        //scrivo nella pipe le informazioni aggiornate.
        write(pipeout,&pos_navicella,sizeof(pos_navicella));
    }
}

/**
 * Funzione che si occupa di generare le coordinate di un missile.
 * @param pipeout File descriptor in scrittura della pipe.
 * @param maxx Massimo valore delle X sullo schermo.
 * @param maxy Massimo valore delle Y sullo schermo.
 * @param navx Coordinata X della navicella che spara il missile.
 * @param navy Coordinata Y della navicella che spara il missile.
 * @param diry Direzione verticale del missile (se pari ad 1 il missile va in alto, pari a -1 va verso il basso).
 */
void missile(int pipeout, int maxx, int maxy, int navx, int navy, int diry){

    Position pos_missile;
    //assegno le informazioni del missile alla variabile pos_missile.
    pos_missile.x=5+navx;
    pos_missile.y=1+navy;
    pos_missile.i=Missile;
    pos_missile.pid=getpid();
    int i=0;
    //definisco quale dei due missili sparati dalla navicella si tratta.
    if(diry==1){
        pos_missile.id=0;
    }
    else{
        pos_missile.id=1;
    }
    //scrivo nella pipe le informazioni iniziali.
    write(pipeout, &pos_missile, sizeof(pos_missile));
    //il missile continua a muoversi fino a che non raggiunge il limite destro dello schermo.
    while(!(pos_missile.x>maxx)){
        //effettuo lo spostamento del missile.
        if(pos_missile.y+diry>maxy || pos_missile.y+diry<2) {
            diry=-diry;
        }
        if(i%6==0){
            pos_missile.y+=diry;
        }
        pos_missile.x+=PASSO;
        i++;
        //scrivo nella pipe le informazioni aggiornate.
        write(pipeout, &pos_missile, sizeof(pos_missile));
        //delay dello spostamento del missile.
        usleep(DELAY_MISSILE);
    }
    //il processo viene terminato quando esce dal while.
    exit(1);

}