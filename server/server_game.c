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
typedef struct{
    int time;
    int now;
    int heal;
    int scene;
    int point[2];
    int defe[2][2];
}GAME;

PAD speed={0,0};
GAME game = {0,0,0,1,{0,0}, {{0,0},{0,0}} };

extern int  def_ugoki(int i);
extern void  def_ugoki2();
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

//ディフェンダーの目指す場所２
extern void def_ugoki2(){
    if(game.defe[0][1] > p[4].x)
        game.defe[0][0] = 1;
    else
        game.defe[0][0] = -1;

    if(game.defe[1][1] > p[5].x)
        game.defe[1][0] = 1;
    else
        game.defe[1][0] = -1;
}


/* パッドの動きを計算する */
void field_set(void){
    struct timespec time_tmp;
    clock_gettime(CLOCK_REALTIME, &time_tmp);
    game.now = time_tmp.tv_sec;

    if(p[4].hp > 0 && p[4].x > -FIELD_W+DEF_W-game.defe[0][0] && p[4].x < FIELD_W-DEF_W-game.defe[0][0] && game.defe[0][0]*(p[4].x - game.defe[0][1]) < 0)
        p[4].x += game.defe[0][0];
    if(p[5].hp > 0 && p[5].x > -FIELD_W+DEF_W-game.defe[1][0] && p[5].x < FIELD_W-DEF_W-game.defe[1][0] && game.defe[1][0]*(p[5].x - game.defe[1][1]) < 0)
	p[5].x += game.defe[1][0];

    int i,j;
    float k,l;
    pad.x += speed.x;
    pad.y += speed.y;
    /* 乱数初期化 */
    srand(time(NULL));
    if(game.scene == 0){
        if(game.heal++>120){
            for(i = 0;i < 4;i++){
                if(++p[i].ap > 100)
                    p[i].ap = 100;
                if(p[i].type == 2 || p[i].type == 3)
                    p[i].hp += 3;
		if(p[i].hp > bai(p[i].type + 6))
		    p[i].hp = bai(p[i].type + 6);
            }
            game.heal = 0;
        }
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
                        game.defe[1][1] = def_ugoki(1);
		  	game.defe[0][1] = 0;
			def_ugoki2();
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
                        game.defe[1][1] = def_ugoki(1);
		  	game.defe[0][1] = 0;
			def_ugoki2();
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
                        game.defe[1][1] = def_ugoki(1);
		  	game.defe[0][1] = 0;
			def_ugoki2();
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
                        game.defe[0][1] = def_ugoki(-1);
		  	game.defe[1][1] = 0;
			def_ugoki2();
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
                        game.defe[0][1] = def_ugoki(-1);
		  	game.defe[1][1] = 0;
			def_ugoki2();
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
                        game.defe[0][1] = def_ugoki(-1);
		  	game.defe[1][1] = 0;
			def_ugoki2();
                }
            }
        }

        //縦の壁にぶつかった時
        if(pad.y + PAD_R > FIELD_H){
            if(pad.x >= GOAL_W || pad.x <= -GOAL_W){
                pad.y = 2 * FIELD_H - pad.y - 2*PAD_R;
                speed.y = speed.y * (-1);
                    game.defe[1][1] = def_ugoki(1);
		    game.defe[0][1] = 0;
		    def_ugoki2();
            }else{
                game.time = game.now;
                game.scene = 1;
                if(++game.point[0] == 2)
                    fprintf(stderr,"lose\n");
                    fprintf(stderr,"lose\n");
                    fprintf(stderr,"lose\n");
                    fprintf(stderr,"lose\n");

            }
        }else if(pad.y - PAD_R <= (-1)*FIELD_H){
            if(pad.x >= GOAL_W || pad.x <= -GOAL_W){
                pad.y = (-2) * FIELD_H - pad.y + 2*PAD_R;
                speed.y = speed.y * (-1);
                    game.defe[0][1] = def_ugoki(-1);
		    game.defe[1][1] = 0;
		    def_ugoki2();
            }else{
                game.time = game.now;
                game.scene = 1;
                if(++game.point[1] == 2)
                    fprintf(stderr,"win");
            }
        }
    }else{	
	if(game.now - game.time > 2 && game.scene == 1){
	    game.scene = 2;
	    p[0].hp = 650;
	    p[0].x = 0;
	    p[1].hp = 300;
	    p[1].x = 0;
 	    p[2].hp = 550;
	    p[2].x = 0;
 	    p[3].hp = 450;
	    p[3].x = 0;
	    p[4].hp = 800;
	    p[4].x = 0;
	    p[5].hp = 800;
	    p[5].x = 0;
   	    pad.x = 0;pad.y = 0;
	    speed.x = 0;speed.y = 0;
            if(game.point[0] == 2 || game.point[1] == 2){
                game.point[0] = 0;
                game.point[1] = 0;
                for(i =0;i<4;i++)
                    p[i].ap = 0;
            }
	}
	if(game.now - game.time > 5){
            game.now = 0;
	    game.scene = 0;
            k = M_PI * (10 + rand()%180)/100;
            while( k < 1.1 && k > 0.9){
                k = M_PI * (10 + rand()%180)/100;
            }
            speed.y = 3 * sin(k);
            speed.x = 3 * cos(k);
	    if(speed.y > 0){
                game.defe[0][1] = def_ugoki(-1);
	        game.defe[1][1] = 0;
		def_ugoki2();
	    }
	    if(speed.y < 0){
                game.defe[1][1] = def_ugoki(1);
	        game.defe[0][1] = 0;
		def_ugoki2();
	    }
	}
    }
}
