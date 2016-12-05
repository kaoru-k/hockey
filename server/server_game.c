/*************************************
  server_game.c
  サーバのゲームモジュール
  徳島大学 工学部 知能情報工学科 27班
*************************************/
#ifdef TEST
#include "server.h"
#else
#include "../client/client.h"
#endif

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
    if(pad.speed_x < 0)
        b = -1;
    a = (-i*DEF_Y - pad.y + i*PAD_R)/pad.speed_y * pad.speed_x + pad.x;
    if(a - PAD_R <= FIELD_W && PAD_R + a >= -FIELD_W){
        return a;
    }else{
        a = a - b * (FIELD_W - PAD_R);
        if( (a / (b* (2*FIELD_W - 2*PAD_R))) % 2 == 0)
            return  b*(FIELD_W - PAD_R) - a % (2*(FIELD_W - PAD_R));
        else
            return b*(-FIELD_W + PAD_R) + a % (2*(FIELD_W -PAD_R));   
    }
}

/* パッドの動きを計算する */
void field_set(void){
    int i,j;
    pad.x += pad.speed_x;
    pad.y += pad.speed_y;
    
    //横の壁にぶつかった時
    if(pad.x + PAD_R > FIELD_W){
        pad.x = 2 * FIELD_W - pad.x - 2 * PAD_R;
        pad.speed_x = pad.speed_x * (-1);
    }else if(pad.x - PAD_R < (-1)*FIELD_W){
 	pad.x = (-2) * FIELD_W - pad.x + 2 * PAD_R;
        pad.speed_x = pad.speed_x * (-1);
    }
    //プレイヤーにぶつかった時
    if(pad.speed_y > 0){
        if(pad.y + PAD_R >= ATK_Y && pad.y + PAD_R <= ATK_Y + pad.speed_y){
            for(i = 0;i < 2;i++){
                if(p[i].type == 0)
                    break;
            }
            if(pad.x > p[i].x - ATK_W && pad.x < p[i].x + ATK_W){
                pad.speed_y = pad.speed_y * (-1);
                pad.speed_x = pad.speed_x * 1;  //跳ね返りの計算
                p[5].x = def_ugoki(1);
                if( (p[i].hp -= pad.speed_y*1) <= 0 ){      //ｈｐ減少
                    p[i].x = 1000;//HPが0以下になった時の処理
                }
            }
        }else if(pad.y + PAD_R >= SUP_Y && pad.y + PAD_R <= SUP_Y + pad.speed_y){
            for(i = 0;i < 2;i++){
                if(p[i].type == 1)
                    break;
            }
            if(pad.x > p[i].x - SUP_W && pad.x < p[i].x + SUP_W){
                pad.speed_y = pad.speed_y * (-1);
                p[5].x = def_ugoki(1);
                if( (p[i].hp -= pad.speed_y*1) <= 0 ){      //ｈｐ減少
                    i = i;//HPが0以下になった時の処理
                }
                if(i == 0){                   //ｈｐ回復
                    p[1].hp += 10;
                }else{
                    p[0].hp += 10;
                }
                p[2].hp += 10;
            }
        }else if(pad.y + PAD_R >= DEF_Y && pad.y + PAD_R <= DEF_Y + pad.speed_y){
            if(pad.x > p[4].x - DEF_W && pad.x < p[4].x + DEF_W){
                pad.speed_y = pad.speed_y * (-1);
                pad.speed_x = pad.speed_x * 1;  //跳ね返りの計算
                p[5].x = def_ugoki(1);
                if( (p[4].hp -= pad.speed_y*1) <= 0 ){      //ｈｐ減少
                    p[4].x = 1000;//HPが0以下になった時の処理
                }
            }
        }
    }else{
	if(pad.y - PAD_R <= -ATK_Y && pad.y - PAD_R >= -ATK_Y + pad.speed_y){
            for(i = 0;i < 2;i++){
                if(p[i+2].type == 0)
                    break;
            }
            if(pad.x > p[i+2].x - ATK_W && pad.x < p[i+2].x + ATK_W){
                pad.speed_y = pad.speed_y * (-1);
                pad.speed_x = pad.speed_x * 1;  //跳ね返りの計算
                p[4].x = def_ugoki(-1);
                if( (p[i+2].hp -= pad.speed_y*10) <= 0 ){      //ｈｐ減少
                    p[i+2].x = 1000;//HPが0以下になった時の処理
                }
            }
        }else if(-pad.y + PAD_R >= SUP_Y && -pad.y + PAD_R <= SUP_Y - pad.speed_y){
            for(i = 0;i < 2;i++){
                if(p[i+2].type == 1)
                    break;
            }
            if(pad.x > p[i+2].x - SUP_W && pad.x < p[i+2].x + SUP_W){
                pad.speed_y = pad.speed_y * (-1);
                p[4].x = def_ugoki(-1);
                if( (p[i+2].hp -= pad.speed_y*1) <= 0 ){      //ｈｐ減少
                    i = i;//HPが0以下になった時の処理
                }
                if(i+2 == 0){                   //ｈｐ回復
                    p[1].hp += 10;
                }else{
                    p[0].hp += 10;
                }
                p[2].hp += 10;
            }
        }else if(-pad.y + PAD_R >= DEF_Y && -pad.y + PAD_R <= DEF_Y - pad.speed_y){
            if(pad.x > p[5].x - DEF_W && pad.x < p[5].x + DEF_W){
                pad.speed_y = pad.speed_y * (-1);
                pad.speed_x = pad.speed_x * 1;  //跳ね返りの計算
                p[4].x = def_ugoki(-1);
                if( (p[5].hp -= pad.speed_y*1) <= 0 ){      //ｈｐ減少
                    p[5].x = 1000;//HPが0以下になった時の処理
                }
            }
        }
    }


    //縦の壁にぶつかった時
    if(pad.y + PAD_R > FIELD_H){
	if(pad.x >= GOAL_W || pad.x <= -GOAL_W){
            pad.y = 2 * FIELD_H - pad.y - 2*PAD_R;
            pad.speed_y = pad.speed_y * (-1);
            p[5].x = def_ugoki(1);
	}else{
	    pad.speed_x = 0;
	    pad.speed_y = 0;
 	}
    }else if(pad.y - PAD_R <= (-1)*FIELD_H){
	if(pad.x >= GOAL_W || pad.x <= -GOAL_W){
 	    pad.y = (-2) * FIELD_H - pad.y + 2*PAD_R;
            pad.speed_y = pad.speed_y * (-1);
            p[4].x = def_ugoki(-1);
	}else{
	    pad.speed_x = 0;
	    pad.speed_y = 0;
 	}
    }
}
