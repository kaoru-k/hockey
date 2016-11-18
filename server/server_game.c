/*************************************
  server_game.c
  サーバのゲームモジュール
  徳島大学 工学部 知能情報工学科 27班
*************************************/

#include "server.h"

static void field_set(void);

/* パッドの動きを計算する */
void field_set(void){
    int i,j;
    pad.x += pad.speed_x;
    pad.y += pad.speed_y;
    
    int a = 0;
    if(pad.speed_y < 0){
        pad.speed_y = pad.speed_y * (-1);
        pad.y = pad.y * (-1);
        a =1;
    }
    
    //横の壁にぶつかった時
    if(pad.x + PAD_R >= FIELD_W){
        pad.x = 2 * FIELD_W - pad.x;
        pad.speed_x = pad.speed_x * (-1);
    }

    //プレイヤーにぶつかった時
    for(j = 0;j < 2;i++){
        if(a == 1)//逆向き
            i = j;
        else
            i = j + 2;
        if(pad.y + PAD_R <= zahyo(p[i].type) && pad.y + PAD_R <= zahyo(p[i].type) + 10 ){
            if(pad.x + PAD_R >= p[i].x && pad.x + PAD_R <= p[i].x + haba(p[i].type) ){
                pad.speed_y = pad.speed_y * (-1) * bai(p[i].type);  //倍率をかけて返す
                p[i].hp -= pad.speed_y;      //HP減少
                if(p[i].type == 0)           //アタッカーの返す向き
                    pad.speed_x = 0;
                else if(p[i].type == 1){
                    i = i;                   //HP回復
                }
            }
        }
    }

    //縦の壁にぶつかった時
    if(pad.y + PAD_R >= FIELD_H){
        if(pad.x <= GOAL_W && pad.x >= GOAL_W * (-1) )  //ゴールした時
            pad.x = pad.x * 1;
        else{
        pad.y = 2 * FIELD_H - pad.y;
        pad.speed_y = pad.speed_y * (-1);
        }
    }

    if(a == 1){
        pad.speed_y = pad.speed_y * (-1);
        pad.y = pad.y * (-1);
    }
}

//キャラの幅を返す関数
int haba(int type){
    switch(type){
    case 0:return ATK_W;
    case 1:return SUP_W;
    case 2:return DEF_W;
    }
}

int zahyo(int type){
    switch(type){
    case 0:return ATK_Y;
    case 1:return SUP_Y;
    case 2:return DEF_Y;
    }
}

//倍率を返す関数
int bai(int type){
    return 1;

}
