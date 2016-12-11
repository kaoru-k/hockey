/*************************************
  server_game.c
  サーバのゲームモジュール
  徳島大学 工学部 知能情報工学科 27班
*************************************/
#include <time.h>
#include <math.h>

#ifndef TEST
#include "server.h"
#else
#include "../client/client.h"
#endif

PAD speed={3,3};

static int game_scene;
static int ti;

extern int  def_ugoki(int i);
extern void field_set(void);
static float  bai  (int type);

//倍率を返す関数
static float bai(int type){
    switch(type){
    case 0:return 1.1;
    case 1:return 1.17;
    case 2:return 1;
    case 3:return 0.85;
    case 4:return 4;
    case 5:return 7;
    default: return 0;
    }
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
//時間	
    time_t timer;
    struct tm *now;

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
                    if(p[i].type == 0 || p[i].type == 1)
                        break;
                }
                if(pad.x + PAD_R > p[i].x - ATK_W && pad.x - PAD_R< p[i].x + ATK_W){
                    if( (p[i].hp -= speed.y) <= 0 ){      //ｈｐ減少
                        p[i].x = 1000;//HPが0以下になった時の処理
                    }
                    k = M_PI * (10 + rand()%80)/100;
                    l = sqrt(speed.x*speed.x + speed.y * speed.y);
                    speed.y = -l * sin(k) * bai(p[i].type);
                    speed.x = l * cos(k) * bai(p[i].type);
                    if(p[5].hp > 0)
                        p[5].x = def_ugoki(1);
                }
            }else if(pad.y + PAD_R >= SUP_Y && pad.y + PAD_R <= SUP_Y + speed.y){
                for(i = 0;i < 2;i++){
                    if(p[i].type == 2 || p[i].type == 3)
                        break;
                }
                if(pad.x + PAD_R > p[i].x - SUP_W && pad.x - PAD_R < p[i].x + SUP_W){
                    if( (p[i].hp -= speed.y) <= 0 ){      //ｈｐ減少
                        p[i].x = 1000;//HPが0以下になった時の処理
                    }
                    if( (speed.y = -speed.y * bai(p[i].type)) > -1)
                        speed.y = -1;
                    if(p[5].hp > 0)
                        p[5].x = def_ugoki(1);
                    if(i == 0){                   //ｈｐ回復
                        p[1].hp += bai(p[i].type+2);
                    }else{
                        p[0].hp += bai(p[i].type+2);
                    }
                    p[2].hp += bai(p[i].type+2);
                }
            }else if(pad.y + PAD_R >= DEF_Y && pad.y + PAD_R <= DEF_Y + speed.y){
                if(pad.x + PAD_R> p[4].x - DEF_W && pad.x-PAD_R < p[4].x + DEF_W){
                    if( (p[4].hp -= speed.y) <= 0 ){      //ｈｐ減少
                        p[4].x = 1000;//HPが0以下になった時の処理
                    }
                    if( (speed.y = speed.y * (-0.9)) > -1)
                        speed.y = -1;
                    if(p[5].hp > 0)
                        p[5].x = def_ugoki(1);
                }
            }
        }else{
            if(pad.y - PAD_R <= -ATK_Y && pad.y - PAD_R >= -ATK_Y + speed.y){
                for(i = 0;i < 2;i++){
                    if(p[i+2].type == 0 || p[i+2].type == 1)
                        break;
                }
                if(pad.x+PAD_R > p[i+2].x - ATK_W && pad.x-PAD_R < p[i+2].x + ATK_W){
                    if( (p[i+2].hp += speed.y) <= 0 ){      //ｈｐ減少
                        p[i+2].x = 1000;//HPが0以下になった時の処理
                    }
                    k = M_PI * (10 + rand()%80)/100;
                    l = sqrt(speed.x*speed.x + speed.y*speed.y);
                    speed.y = l * sin(k) * bai(p[i+2].type);
                    speed.x = l * cos(k) * bai(p[i+2].type);
                    if(p[4].hp > 0)
                        p[4].x = def_ugoki(-1);
                }
            }else if(-pad.y + PAD_R >= SUP_Y && -pad.y + PAD_R <= SUP_Y - speed.y){
                for(i = 0;i < 2;i++){
                    if(p[i+2].type == 2 || p[i+2].type == 3)
                        break;
                }
                if(pad.x+PAD_R > p[i+2].x - SUP_W && pad.x-PAD_R < p[i+2].x + SUP_W){
                    if( (p[i+2].hp += speed.y) <= 0 ){      //ｈｐ減少
                        p[i+2].x = 1000;//HPが0以下になった時の処理
                    }
                    if( (speed.y = speed.y * -bai(p[i+2].type)) < 1)
                        speed.y = 1;
                    if(p[4].hp > 0)
                        p[4].x = def_ugoki(-1);
                    if(i == 0){                   //ｈｐ回復
                        if(p[3].hp > 0)
                            p[3].hp += bai(p[i+2].type+2);
                    }else{
                        if(p[2].hp > 0)
                            p[2].hp += bai(p[i+2].type+2);
                    }
                    if(p[5].hp > 0)
                        p[5].hp += bai(p[i+2].type+2);
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
/* 現在時刻の取得 */
      		time(&timer);
     		now = localtime(&timer);
     		ti = now->tm_sec + now->tm_hour * 3600 + now->tm_min * 60;
                game_scene = 1;
            }
        }else if(pad.y - PAD_R <= (-1)*FIELD_H){
            if(pad.x >= GOAL_W || pad.x <= -GOAL_W){
                pad.y = (-2) * FIELD_H - pad.y + 2*PAD_R;
                speed.y = speed.y * (-1);
                if(p[4].hp > 0)
                    p[4].x = def_ugoki(-1);
            }else{
/* 現在時刻の取得 */
      		time(&timer);
     		now = localtime(&timer);
     		ti = now->tm_sec + now->tm_hour * 3600 + now->tm_min * 60;
                game_scene = 1;
            }
        }
    }else{
/* 現在時刻の取得 */
        time(&timer);
        now = localtime(&timer);
        int tim = now->tm_sec + now->tm_hour * 3600 + now->tm_min * 60 ;
	if(tim - ti > 5){
	    game_scene = 0;
	    p[0].type = 0;
	    p[0].hp = 65;
	    p[0].ap = 0;
	    p[0].x = 0;
            p[1].type = 2;
	    p[1].hp = 30;
	    p[1].ap = 0;
	    p[1].x = 0;
   
            p[2].type = 1;
 	    p[2].hp = 55;
	    p[2].ap = 0;
	    p[2].x = 0;
              p[3].type = 3;
 	    p[3].hp = 45;
	    p[3].ap = 0;
	    p[3].x = 0;
              p[4].type = 4,
	    p[4].hp = 80;
	    p[4].ap = 0;
	    p[4].x = 0;
               p[5].type = 4;
	    p[5].hp = 80;
	    p[5].ap = 0;
	    p[5].x = 0;
   	    pad.x = 0;pad.y = 0;
	    speed.x = 3;speed.y = 3;
	}
    }
    fprintf(stderr, "x:%f y:%f\n", pad.x, pad.y);
}
