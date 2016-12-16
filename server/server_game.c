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
int game_scene = 1;
float gt;
float ti;
int heal = 0;

extern int  def_ugoki(int i);
extern void field_set(void);
static float  bai  (int type);

//倍率を返す関数
static float bai(int type){
    switch(type){
    case 0:return 1.1;  //跳ね返り
    case 1:return 1.17;
    case 2:return 1;
    case 3:return 0.85;
    case 4:return 40;   //回復
    case 5:return 60;
    case 6:return 650;  //maxhp
    case 7:return 550;
    case 8:return 300;
    case 9:return 450;
    case 10:return 800;
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

/* 現在時刻の取得 */
    time(&timer);
    now = localtime(&timer);
    int tim = now->tm_sec + now->tm_hour * 3600 + now->tm_min * 60 ;
    
    int i,j;
    float k,l;
    pad.x += speed.x;
    pad.y += speed.y;
    
    if(heal++>120){
	for(i = 0;i < 4;i++){
	    if(++p[i].ap > 100)
		p[i].ap = 100;
            if(p[i].type == 2 || p[i].type == 3)
		p[i].hp += 3;
		if(p[i].hp > bai(p[i].type + 6))
		    p[i].hp = bai(p[i].type + 6);
	}
	heal = 0;
    }

    /* 乱数初期化 */
    srand(time(NULL));
    
    if(game_scene == 0){
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
		    if( (p[i].ap += sqrt(speed.x*speed.x + speed.y * speed.y)) > 100)
			p[i].ap = 100;
                    if( (p[i].hp -= sqrt(speed.x*speed.x + speed.y * speed.y)*10) <= 0 ){      //ｈｐ減少
			if(p[4].hp <= 0){
                            p[i].x = 1000;//HPが0以下になった時の処理
			    p[i].hp = 0;
			}
			if(p[4].hp > 0){
			    p[i].hp = p[4].hp;
			    p[4].hp = 0;
			    p[4].x = 1000;
			}			
                    }
                    if( (l = sqrt(speed.x*speed.x + speed.y * speed.y)) < 7){
                    	k = M_PI * (10 + rand()%80)/100;
                   	speed.y = -l * sin(k) * bai(p[i].type);
                    	speed.x = l * cos(k) * bai(p[i].type);
		    }else{
                        k = M_PI * (10 + rand()%80)/100;
                    	speed.y = -l * sin(k);
                    	speed.x = l * cos(k);
		    }
                    if(p[5].hp > 0)
                        p[5].x = def_ugoki(1);
                }
            }else if(pad.y + PAD_R >= SUP_Y && pad.y + PAD_R <= SUP_Y + speed.y){
                for(i = 0;i < 2;i++){
                    if(p[i].type == 2 || p[i].type == 3){
                        break;
		    }
                }
                if(pad.x + PAD_R > p[i].x - SUP_W && pad.x - PAD_R < p[i].x + SUP_W){
		    if( (p[i].ap += sqrt(speed.x*speed.x + speed.y * speed.y)) > 100)
			p[i].ap = 100;
                    if( (p[i].hp -= sqrt(speed.x*speed.x + speed.y * speed.y)*10) <= 0 ){      //ｈｐ減少
			if(p[4].hp <= 0){
                            p[i].x = 1000;//HPが0以下になった時の処理
			    p[i].hp = 0;
			}
			if(p[4].hp > 0){
			    p[i].hp = p[4].hp;
			    p[4].hp = 0;
			    p[4].x = 1000;
			}
                    }
                    if(sqrt(speed.x*speed.x + speed.y * speed.y) > 2){ 
                    	speed.y = -speed.y * bai(p[i].type);
		        speed.x = speed.x * bai(p[i].type);
		    }else
			speed.y = -speed.y;
                    if(p[5].hp > 0)
                        p[5].x = def_ugoki(1);
                    if(i == 0){                   //ｈｐ回復
			if(p[1].hp > 0)
                            p[1].hp += bai(p[i].type+2);
		        if(p[1].hp > bai(p[1].type + 6))
		            p[1].hp = bai(p[1].type + 6);
                    }else{
			if(p[0].hp > 0)
                            p[0].hp += bai(p[i].type+2);
		        if(p[0].hp > bai(p[0].type + 6))
		            p[0].hp = bai(p[0].type + 6);
                    }
		    if(p[4].hp > 0)
			p[4].hp += bai(p[i].type+2);
		    if(p[4].hp > bai(p[4].type + 6))
		        p[4].hp = bai(p[4].type + 6);
                }
            }else if(pad.y + PAD_R >= DEF_Y && pad.y + PAD_R <= DEF_Y + speed.y){
                if(pad.x + PAD_R> p[4].x - DEF_W && pad.x-PAD_R < p[4].x + DEF_W){
                    if( (p[4].hp -= sqrt(speed.x*speed.x + speed.y * speed.y)*10) <= 0 ){      //ｈｐ減少
                        p[4].x = 1000;//HPが0以下になった時の処理
			p[4].hp = 0;
                    }
                    if(sqrt(speed.x*speed.x + speed.y * speed.y) > 2){ 
                    	speed.y = -speed.y * 0.9;
		        speed.x = speed.x * 0.9;
		    }else
			speed.y = -speed.y;
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
		    if( (p[i+2].ap += sqrt(speed.x*speed.x + speed.y * speed.y)) > 100)
			p[i+2].ap = 100;
                    if( (p[i+2].hp -= sqrt(speed.x*speed.x + speed.y * speed.y)*10) <= 0 ){      //ｈｐ減少
			if(p[5].hp <= 0){    
                            p[i+2].x = 1000;//HPが0以下になった時の処理
			    p[i+2].hp = 0;
			}
			if(p[5].hp > 0){
			    p[i+2].hp = p[5].hp;
			    p[5].hp = 0;
			    p[5].x = 1000;
			}
                    }
                    if( (l = sqrt(speed.x*speed.x + speed.y*speed.y)) < 7){
                        k = M_PI * (10 + rand()%80)/100;
                    	speed.y = l * sin(k) * bai(p[i+2].type);
                    	speed.x = l * cos(k) * bai(p[i+2].type);
		    }else{
                        k = M_PI * (10 + rand()%80)/100;
                    	speed.y = l * sin(k);
                    	speed.x = l * cos(k);
		    }
                    if(p[4].hp > 0)
                        p[4].x = def_ugoki(-1);
                }
            }else if(-pad.y + PAD_R >= SUP_Y && -pad.y + PAD_R <= SUP_Y - speed.y){
                for(i = 0;i < 2;i++){
                    if(p[i+2].type == 2 || p[i+2].type == 3)
                        break;
                }
                if(pad.x+PAD_R > p[i+2].x - SUP_W && pad.x-PAD_R < p[i+2].x + SUP_W){
		    if( (p[i+2].ap += sqrt(speed.x*speed.x + speed.y * speed.y)) > 100)
			p[i+2].ap = 100;
                    if( (p[i+2].hp -= sqrt(speed.x*speed.x + speed.y * speed.y)*10) <= 0 ){      //ｈｐ減少
			if(p[5].hp <= 0){
                            p[i+2].x = 1000;//HPが0以下になった時の処理
			    p[i+2].hp = 0;
			}
			if(p[5].hp > 0){
			    p[i+2].hp = p[5].hp;
			    p[5].hp = 0;
			    p[5].x = 1000;
			}
                    }
                    if(sqrt(speed.x*speed.x + speed.y * speed.y) > 2){ 
                    	speed.y = -speed.y * bai(p[i+2].type);
		        speed.x = speed.x * bai(p[i+2].type);
		    }else
			speed.y = -speed.y;
                    if(p[4].hp > 0)
                        p[4].x = def_ugoki(-1);
                    if(i == 0){                   //ｈｐ回復
                        if(p[3].hp > 0)
                            p[3].hp += bai(p[i+2].type+2);
		        if(p[3].hp > bai(p[3].type + 6))
		            p[3].hp = bai(p[3].type + 6);
                    }else{
                        if(p[2].hp > 0)
                            p[2].hp += bai(p[i+2].type+2);
		        if(p[2].hp > bai(p[2].type + 6))
		            p[2].hp = bai(p[2].type + 6);
                    }
                    if(p[5].hp > 0)
                        p[5].hp += bai(p[i+2].type+2);
		    if(p[5].hp > bai(p[5].type + 6))
		        p[5].hp = bai(p[5].type + 6);
                }
            }else if(-pad.y + PAD_R >= DEF_Y && -pad.y + PAD_R <= DEF_Y - speed.y){
                if(pad.x+PAD_R > p[5].x - DEF_W && pad.x-PAD_R < p[5].x + DEF_W){
                    if( (p[5].hp -= sqrt(speed.x*speed.x + speed.y * speed.y)*10) <= 0 ){      //ｈｐ減少
                        p[5].x = 1000;//HPが0以下になった時の処理
			p[5].hp = 0;
                    }
                    if(sqrt(speed.x*speed.x + speed.y * speed.y) > 2){ 
                    	speed.y = -speed.y * 0.9;
		        speed.x = speed.x * 0.9;
		    }else
			speed.y = -speed.y;
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
	if(tim - ti > 2 && game_scene == 1){
	    game_scene = 2;
	    p[0].hp = 650;
	    p[0].ap = 0;
	    p[0].x = 0;
	    p[1].hp = 300;
	    p[1].ap = 0;
	    p[1].x = 0;
 	    p[2].hp = 550;
	    p[2].ap = 0;
	    p[2].x = 0;
 	    p[3].hp = 450;
	    p[3].ap = 0;
	    p[3].x = 0;
	    p[4].hp = 800;
	    p[4].ap = 0;
	    p[4].x = 0;
	    p[5].hp = 800;
	    p[5].ap = 0;
	    p[5].x = 0;
   	    pad.x = 0;pad.y = 0;
	    speed.x = 0;speed.y = 0;
	}
	if(tim - ti > 5){	
	    game_scene = 0;
	    speed.x = 3;speed.y = 3;
	}
    }
    fprintf(stderr, "x:%f y:%f\n", pad.x, pad.y);
}
