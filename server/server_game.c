/*************************************
  server_game.c
  サーバのゲームモジュール
  徳島大学 工学部 知能情報工学科 27班
*************************************/
#include <time.h>
#include <math.h>

#ifdef TEST
#include "server.h"
#else
#include "../client/client.h"
#endif

PAD speed={3,3};

static int game_scene;

extern int  def_ugoki(int i);
extern void field_set(void);
static int  haba (int type);
static int  zahyo(int type);
static int  bai  (int type);


//キャラの幅を返す関数
static int haba(int type){
    switch(type){
    case 0:return ATK_W;
    case 1:return SUP_W;
    case 2:return DEF_W;
    }
}

static int zahyo(int type){
    switch(type){
    case 0:return ATK_Y;
    case 1:return SUP_Y;
    case 2:return DEF_Y;
    }
}

//倍率を返す関数
static int bai(int type){
    return 1;

}

//ディフェンダーの目指す場所
extern int def_ugoki(int i){
    int a = 0;
    int b = 1;
    b = 1;
    a = 0;
    if(speed.x < 0)
        b = -1;
    a = (-i*DEF_Y - pad.y + i*PAD_R)/speed.y * speed.x + pad.x;
    if(a - PAD_R <= FIELD_W && PAD_R + a >= -FIELD_W){
        return a;
    }else{
        a = a - b * (FIELD_W - PAD_R);
        if( (a / (b* (2*FIELD_W - 2*PAD_R))) % 2 == 0)
            return b*(FIELD_W - PAD_R) - a % (2*(FIELD_W - PAD_R));
        else
            return b*(-FIELD_W + PAD_R) + a % (2*(FIELD_W -PAD_R));   
    }
}

/* パッドの動きを計算する */
void field_set(void){
    int i,j;
    float k,l;
    pad.x += speed.x;
    pad.y += speed.y;

    /* 乱数初期化 */
    srand(time(NULL));
    
    if(game_scene != 1){
        //横の壁にぶつかった時
        if(pad.x + PAD_R > FIELD_W){
            pad.x = 2 * FIELD_W - pad.x - 2 * PAD_R;
            speed.x = speed.x * (-1);
        }else if(pad.x - PAD_R < (-1)*FIELD_W){
            pad.x = (-2) * FIELD_W - pad.x + 2 * PAD_R;
            speed.x = speed.x * (-1);
        }
        //プレイヤーにぶつかった時
        if(speed.y > 0){
            if(pad.y + PAD_R >= ATK_Y && pad.y + PAD_R <= ATK_Y + speed.y){
                for(i = 0;i < 2;i++){
                    if(p[i].type == 0)
                        break;
                }
                if(pad.x + PAD_R > p[i].x - ATK_W && pad.x - PAD_R< p[i].x + ATK_W){
                    if( (p[i].hp -= speed.y) <= 0 ){      //ｈｐ減少
                        p[i].x = 1000;//HPが0以下になった時の処理
                    }
                    k = M_PI * (10 + rand()%80)/100;
                    l = sqrt(speed.x*speed.x + speed.y * speed.y);
                    speed.y = -l * sin(k) * 1.1;
                    speed.x = l * cos(k) * 1.1;
                    if(p[5].hp > 0)
                        p[5].x = def_ugoki(1);   
                }
            }else if(pad.y + PAD_R >= SUP_Y && pad.y + PAD_R <= SUP_Y + speed.y){
                for(i = 0;i < 2;i++){
                    if(p[i].type == 1)
                        break;
                }
                if(pad.x + PAD_R > p[i].x - SUP_W && pad.x - PAD_R < p[i].x + SUP_W){
                    if( (p[i].hp -= speed.y*5) <= 0 ){      //ｈｐ減少
                        p[i].x = 1000;//HPが0以下になった時の処理
                    }
                    if( (speed.y = speed.y * (-0.8)) > -1)
                        speed.y = -1;
                    if(p[5].hp > 0)
                        p[5].x = def_ugoki(1);
                    if(i == 0){                   //ｈｐ回復
                        p[1].hp += 10;
                    }else{
                        p[0].hp += 10;
                    }
                    p[2].hp += 10;
                }
            }else if(pad.y + PAD_R >= DEF_Y && pad.y + PAD_R <= DEF_Y + speed.y){
                if(pad.x + PAD_R> p[4].x - DEF_W && pad.x-PAD_R < p[4].x + DEF_W){
                    if( (p[4].hp -= speed.y) <= 0 ){      //ｈｐ減少
                        p[4].x = 1000;//HPが0以下になった時の処理
                    }
                    if( (speed.y = speed.y * (-0.9)) > -1)
                        speed.y = -1;
                    speed.x = speed.x * 1;  //跳ね返りの計算
                    if(p[5].hp > 0)
                        p[5].x = def_ugoki(1);
                }
            }
        }else{
            if(pad.y - PAD_R <= -ATK_Y && pad.y - PAD_R >= -ATK_Y + speed.y){
                for(i = 0;i < 2;i++){
                    if(p[i+2].type == 0)
                        break;
                }
                if(pad.x+PAD_R > p[i+2].x - ATK_W && pad.x-PAD_R < p[i+2].x + ATK_W){
                    if( (p[i+2].hp += speed.y) <= 0 ){      //ｈｐ減少
                        p[i+2].x = 1000;//HPが0以下になった時の処理
                    }
                    k = M_PI * (10 + rand()%80)/100;
                    l = sqrt(speed.x*speed.x + speed.y*speed.y);
                    speed.y = l * sin(k) * 1.1;
                    speed.x = l * cos(k) * 1.1;
                    if(p[4].hp > 0)
                        p[4].x = def_ugoki(-1);
                }
            }else if(-pad.y + PAD_R >= SUP_Y && -pad.y + PAD_R <= SUP_Y - speed.y){
                for(i = 0;i < 2;i++){
                    if(p[i+2].type == 1)
                        break;
                }
                if(pad.x+PAD_R > p[i+2].x - SUP_W && pad.x-PAD_R < p[i+2].x + SUP_W){
                    if( (p[i+2].hp += speed.y*5) <= 0 ){      //ｈｐ減少
                        p[i+2].x = 1000;//HPが0以下になった時の処理
                    }
                    if( (speed.y = speed.y * (-0.8)) < 1)
                        speed.y = 1;
                    if(p[4].hp > 0)
                        p[4].x = def_ugoki(-1);
                    if(i == 0){                   //ｈｐ回復
                        if(p[3].hp > 0)
                            p[3].hp += 10;
                    }else{
                        if(p[2].hp > 0)
                            p[2].hp += 10;
                    }
                    if(p[5].hp > 0)
                        p[5].hp += 10;
                }
            }else if(-pad.y + PAD_R >= DEF_Y && -pad.y + PAD_R <= DEF_Y - speed.y){
                if(pad.x+PAD_R > p[5].x - DEF_W && pad.x-PAD_R < p[5].x + DEF_W){
                    if( (p[5].hp += speed.y) <= 0 ){      //ｈｐ減少
                        p[5].x = 1000;//HPが0以下になった時の処理
                    }
                    if( (speed.y = speed.y * (-0.9)) < 1){
                        speed.y = 1;
                    }
                    speed.x = speed.x * 0.9;  //跳ね返りの計算
                    if(p[4].hp > 0)
                        p[4].x = def_ugoki(-1);
                }
            }
        }

        //縦の壁にぶつかった時
        if(pad.y + PAD_R > FIELD_H){
            if(pad.x >= GOAL_W || pad.x <= -GOAL_W){
                pad.y = 2 * FIELD_H - pad.y - 2*PAD_R;
                speed.y = speed.y * (-1);
                if(p[5].hp > 0)
                    p[5].x = def_ugoki(1);
            }else{
                game_scene = 1;
            }
        }else if(pad.y - PAD_R <= (-1)*FIELD_H){
            if(pad.x >= GOAL_W || pad.x <= -GOAL_W){
                pad.y = (-2) * FIELD_H - pad.y + 2*PAD_R;
                speed.y = speed.y * (-1);
                if(p[4].hp > 0)
                    p[4].x = def_ugoki(-1);
            }else{
                game_scene = 1;
            }
        }
    }
    /*
      fprintf(stderr, "field_set() pad:%f %f\n", pad.x, pad.y);
      fprintf(stderr, "p[0]:%f p[1]:%f p[2]:%f \n", p[0].x, p[1].x, p[2].x);
      fprintf(stderr, "p[3]:%f p[4]:%f p[5]:%f \n", p[3].x, p[4].x, p[5].x);
    */
}
