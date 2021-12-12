#include "engine.h"
#include "navicella.h"

void navicella(int pipeout){
    Position pos_navicella;
    pos_navicella.x=1;
    pos_navicella.y=-1;
    pos_navicella.sprite=SpriteNavicella;

    pipe(pipeout, &pos_navicella, sizeof(pos_navicella));

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
                break
        }
        write(pipeout,&pos_navicella,sizeof(pos_navicella));
    }
}

void StampaNavicella(){
    printf("%s", SpriteNavicella);
}