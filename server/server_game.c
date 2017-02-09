/*************************************
  server_game.c
  サーバのゲームモジュール
  徳島大学 工学部 知能情報工学科 27班
*************************************/
#include "server.h"
#include <time.h>
#include <math.h>

PAD speed={0,0};
GAME game = {0,0,0,3,{0,0,1}, {{0,0},{0,0}}, {0,0} ,{0,0}, {10,10}};
int win;
int s_flag[4];
static float bai(int type);
static int   def_ugoki(int i);
static void  def_ugoki2(void);
static int   sosei(int a);
static int   hissatu(int type,int a);
static void  Hcom(int id);
struct timespec time_tmp;
int hst[4] = {0};

static float bai(int type)
{
/*************************************
倍率を返す関数
そのまま	: 跳ね返り
+4	: 回復力　　（サポーターのみ）
+6	: HP
*************************************/
    switch(type){
    case 0:return 1.1;  //跳ね返り
    case 1:return 1.14;
    case 2:return 0.85;
    case 3:return 1;
    case 4:return 80;   //回復
    case 5:return 60;
    case 6:return ATK1_HP0;  //maxhp
    case 7:return ATK2_HP0;
    case 8:return SUP1_HP0;
    case 9:return SUP2_HP0;
    case 10:return DEF_HP0;
    default: return 0;
    }
}

//ディフェンダーの目指す場所
static int def_ugoki(int i){
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
static void def_ugoki2(){
    if(game.defe[0][1] > p[4].x)
        game.defe[0][0] = 1;
    else
        game.defe[0][0] = -1;

    if(game.defe[1][1] > p[5].x)
        game.defe[1][0] = 1;
    else
        game.defe[1][0] = -1;
}

static int sosei(int a){
    if(a == 1){
    if(p[0].hp == 0){
	p[0].hp = bai(p[0].type+6);
	p[0].x = 0;
	return 0;
    }
        if(p[4].hp == 0){
	    p[4].hp = bai(p[4].type+6);
	    p[4].x = 0;
	    return 0;
        }
    }
    if(a == -1){
    if(p[2].hp == 0){
	p[2].hp = bai(p[2].type+6);
	p[2].x = 0;
	return 0;
    }
	if(p[5].hp == 0){
	    p[5].hp = bai(p[5].type + 6);
	    p[5].x = 0;
	    return 0;
	}
    }
    return -1;
}

//typeキャラタイプ　a : 敵側か味方側か
static int hissatu(int type,int a){
    switch(type){
    case 0:
	if(a == -1)
	    game.co[1] = 10;
	else
            game.co[0] = -10;
	return -1;
    case 1:
	if(a == -1)
            game.co[1] = 1;
	else
	    game.co[0] = 1;
        return -1;
    case 3:
	game.time = game.now;
	if(a == 1)
            game.han[1] = 20;
  	if(a == -1)
	    game.han[0] = 20;
        break;
    case 2:
        return sosei(a);
        break;
    default:break;
    }
    return 0;
}

//必殺コマンド,引数 必殺を使ったキャラのid
static void Hcom(int id){
    int i;
    if(id == 0 || id == 1){
        if((i = hissatu(p[id].type,1)) !=-1){
	    p[id].ap = 0;
	    hst[id] = 1;
	}
        return;
    }
    if(id == 2 || id == 3){
        if((i = hissatu(p[id].type,-1)) != -1){
	    p[id].ap = 0;
	    hst[id] = 1;
	}
        return;
    }
}

void shokika(void){
    int i;
	    p[0].hp = bai(p[0].type+6);
	    p[0].x = 0;
	    p[1].hp = bai(p[1].type+6);
	    p[1].x = 0;
 	    p[2].hp = bai(p[2].type+6);
	    p[2].x = 0;
 	    p[3].hp = bai(p[3].type+6);
	    p[3].x = 0;
	    p[4].hp = 800;
	    p[4].x = 0;
	    p[5].hp = 800;
	    p[5].x = 0;
	    game.defe[0][0] = 0;
	    game.defe[0][1] = 0;
	    game.defe[1][0] = 0;
	    game.defe[1][1] = 0;
	    game.co[0] = 0;
	    game.co[1] = 0;
	    game.han[0] = 10;
	    game.han[1] = 10;
	    speed.x = 0;speed.y = 0;
            if(game.point[0] == max_point || game.point[1] == max_point){//勝敗がついた時
                reset_flag = -1;
                while (reset_flag == -1 && endflag != 1); //プレイヤー選択を待機
                reset_flag = 1;
                game.point[0] = 0;
                game.point[1] = 0;
                clock_gettime(CLOCK_REALTIME, &time_tmp);
                game.time = time_tmp.tv_sec - 2;
                for(i =0;i<4;i++)
                    p[i].ap = 0;
            }
}

int hst_flag(void){
    if(hst[3]+hst[2]+hst[1]+hst[0] > 0)
 	return 1;
    return 0;
}

/* パッドの動きを計算する */
void field_set(void){
    clock_gettime(CLOCK_REALTIME, &time_tmp);
    game.now = time_tmp.tv_sec;

    if(p[4].hp > 0 && p[4].x > -FIELD_W+DEF_W-game.defe[0][0] && p[4].x < FIELD_W-DEF_W-game.defe[0][0] && game.defe[0][0]*(p[4].x - game.defe[0][1]) < 0)
        p[4].x += game.defe[0][0];
    if(p[5].hp > 0 && p[5].x > -FIELD_W+DEF_W-game.defe[1][0] && p[5].x < FIELD_W-DEF_W-game.defe[1][0] && game.defe[1][0]*(p[5].x - game.defe[1][1]) < 0)
	p[5].x += game.defe[1][0];

    int i,j;
    float k,l;

//必殺
    for(i = 0;i < 4;i++){
    	if(p[i].ap == 100 && s_flag[p[i].cid] == 1 && game.scene == 0){
            Hcom(i);
	}
    }

//ゼニヤッタの必殺終了条件
    if(game.han[0] != 10){
	if(game.now - game.time > 30){ //　何秒間持続するか
	    game.han[0] = 10;
	    hst[3] = 0;
	}
    }
    if(game.han[1] != 10){
	if(game.now - game.time > 45){
	    game.han[1] = 10;
	    hst[1] = 0;
	}
    }

    /* 乱数初期化 */
    srand(time(NULL));
    if(game.scene == 0){
    //通常
    pad.x += speed.x;
    pad.y += speed.y;
        if(game.heal++>120){
            for(i = 0;i < 4;i++){
		if(p[i].hp > 0){
                    if(++p[i].ap > 100)
                    	p[i].ap = 100;
                    if(p[i].type == 2)
                    	p[i].hp += 4;
		    if(p[i].type == 3)
		    	p[i].hp += 5;
		    }
		if(p[i].hp > bai(p[i].type + 6))
	    	    p[i].hp = bai(p[i].type + 6);
            }
            game.heal = 0;
        }
        //横の壁にぶつかった時
        if(pad.x + PAD_R > FIELD_W){
            pad.x = 2 * FIELD_W - pad.x - 2 * PAD_R;
            speed.x = speed.x * (-1);
	    sound_flag = 1;
        }else if(pad.x - PAD_R < (-1)*FIELD_W){
            pad.x = (-2) * FIELD_W - pad.x + 2 * PAD_R;
            speed.x = speed.x * (-1);
	    sound_flag = 1;
        }
        //プレイヤーにぶつかった時
        if(speed.y > 0){
            if(pad.y + PAD_R >= ATK_Y && pad.y + PAD_R <= ATK_Y + speed.y){
                if(pad.x + PAD_R > p[0].x - ATK_W && pad.x - PAD_R< p[0].x + ATK_W){
		    sound_flag = 1;
    		    if(game.co[1] == 10 && speed.y == game.co[1]){//必殺
		 	hst[2] = 0;
	    		game.co[1] = 0;
			p[0].hp -= 25*game.han[0];
			speed.x = game.spd[0];
	    		speed.y = game.spd[1];
    		    }
		    if( (p[0].ap += sqrt(speed.x*speed.x + speed.y * speed.y)) > 100)
			p[0].ap = 100;
                    if( (p[0].hp -= sqrt(speed.x*speed.x + speed.y * speed.y)*game.han[0]) <= 0 ){      //ｈｐ減少
			if(p[4].hp <= 0){
                            p[0].x = 1000;//HPが0以下になった時の処理
			    p[0].hp = 0;
			}
			if(p[4].hp > 0){
			    if( (p[0].hp = p[4].hp) > bai(p[0].type+6) )
				p[0].hp = bai(p[0].type+6);
			    p[4].hp = 0;
			    p[4].x = 1000;
			}
                    }
		    if(game.co[0] == 1){
			game.co[0] = 5;
			p[0].ap = 0;
			hst[0] = 1;
		    }
                    if(game.co[1] != -5){
                        if( (l = sqrt(speed.x*speed.x + speed.y * speed.y)) < 7){
                            k = M_PI * (10 + rand()%80)/100;
                            speed.y = -l * sin(k) * bai(p[0].type);
                            speed.x = l * cos(k) * bai(p[0].type);
                        }else{
                            k = M_PI * (10 + rand()%80)/100;
                            speed.y = -l * sin(k);
                            speed.x = l * cos(k);
                        }
                        game.defe[1][1] = def_ugoki(1); /////////////////////////////ここからテスト
                        //game.co = -10;		      ////////////////////
                        if(game.co[0] == -10){
			    p[0].ap = 0;
                            game.spd[0] = speed.x;
                            game.spd[1] = speed.y;
                            speed.x = 0;
                            speed.y = game.co[0];
                            game.defe[1][1] = pad.x;          ///////////////////ここまで
                        }
                        game.defe[0][1] = 0;
                        def_ugoki2();
                    }else{
                        if((p[2].hp += 160) > bai(p[2].type + 6))
                            p[2].hp = bai(p[2].type + 6);
                        game.co[1] = 0;
			hst[2] = 0;
		    }
                }
            }else if(pad.y + PAD_R >= SUP_Y && pad.y + PAD_R <= SUP_Y + speed.y){
                if(pad.x + PAD_R > p[1].x - SUP_W && pad.x - PAD_R < p[1].x + SUP_W){
		    sound_flag = 1;
    		    if(game.co[1] == 10 && speed.y == game.co[1]){//必殺
	    		game.co[1] = 0;
			hst[2] = 0;
			p[1].hp -= 25*game.han[0];
			speed.x = game.spd[0];
	    		speed.y = game.spd[1];
    		    }
		    if( (p[1].ap += sqrt(speed.x*speed.x + speed.y * speed.y)) > 100)
			p[1].ap = 100;
                    if( (p[1].hp -= sqrt(speed.x*speed.x + speed.y * speed.y)*game.han[0]) <= 0 ){      //ｈｐ減少
			if(p[4].hp <= 0){
                            p[1].x = 1000;//HPが0以下になった時の処理
			    p[1].hp = 0;
			}
			if(p[4].hp > 0){
			    if( (p[1].hp = p[4].hp) > bai(p[1].type+6) )
				p[1].hp = bai(p[1].type+6);
			    p[4].hp = 0;
			    p[4].x = 1000;
			}
                    }
                    if(game.co[1] != -5){
                        if(sqrt(speed.x*speed.x + speed.y * speed.y) > 3){ 
                            speed.y = -speed.y * bai(p[1].type);
                            speed.x = speed.x * bai(p[1].type);
                        }else
                            speed.y = -speed.y;
                        game.defe[1][1] = def_ugoki(1);
		  	game.defe[0][1] = 0;
			def_ugoki2();
                        if(p[0].hp > 0)                       //** ｈｐ回復                 
                            p[0].hp += bai(p[1].type+2);
                        if(p[0].hp > bai(p[0].type + 6))
                            p[0].hp = bai(p[0].type + 6);
                        if(p[4].hp > 0)
                            p[4].hp += bai(p[1].type+2);
                        if(p[4].hp > bai(p[4].type + 6))
                            p[4].hp = bai(p[4].type + 6);     //          **//
                    }else{
                        if((p[2].hp += 160) > bai(p[2].type + 6))
                            p[2].hp = bai(p[2].type + 6);
                        game.co[1] = 0;
			hst[2] = 0;
		    }
                }
            }else if(pad.y + PAD_R >= DEF_Y && pad.y + PAD_R <= DEF_Y + speed.y){
                if(pad.x + PAD_R> p[4].x - DEF_W && pad.x-PAD_R < p[4].x + DEF_W){
		    sound_flag = 1;
    		    if(game.co[1] == 10 && speed.y == game.co[1]){//必殺
	    		game.co[1] = 0;
			hst[2] = 0;
			p[4].hp -= 25*game.han[0];
			speed.x = game.spd[0];
	    		speed.y = game.spd[1];
    		    }
                    if( (p[4].hp -= sqrt(speed.x*speed.x + speed.y * speed.y)*game.han[0]) <= 0 ){      //ｈｐ減少
                        p[4].x = 1000;//HPが0以下になった時の処理
			p[4].hp = 0;
                    }
                    if(game.co[1] != -5){
                        if(sqrt(speed.x*speed.x + speed.y * speed.y) > 3){ 
                            speed.y = -speed.y * 0.9;
                            speed.x = speed.x * 0.9;
                        }else
                            speed.y = -speed.y;
                        game.defe[1][1] = def_ugoki(1);
		  	game.defe[0][1] = 0;
			def_ugoki2();
                    }else{
                        if((p[2].hp += 160) > bai(p[2].type + 6))
                            p[2].hp = bai(p[2].type + 6);
                        game.co[1] = 0;
			hst[2] = 0;
		    }
                }
            }
        }else{
            if(pad.y - PAD_R <= -ATK_Y && pad.y - PAD_R >= -ATK_Y + speed.y){
                if(pad.x+PAD_R > p[2].x - ATK_W && pad.x-PAD_R < p[2].x + ATK_W){
		    sound_flag = 1;
    		    if(game.co[0] == -10 && speed.y == game.co[0]){//必殺
	    		game.co[0] = 0;
			hst[0] = 0;
			p[2].hp -= 25*game.han[1];
			speed.x = game.spd[0];
	    		speed.y = game.spd[1];
    		    }
		    if( (p[2].ap += sqrt(speed.x*speed.x + speed.y * speed.y)) > 100)
			p[2].ap = 100;
                    if( (p[2].hp -= sqrt(speed.x*speed.x + speed.y * speed.y)*game.han[1]) <= 0 ){      //ｈｐ減少
			if(p[5].hp <= 0){
                            p[2].x = 1000;//HPが0以下になった時の処理
			    p[2].hp = 0;
			}
			if(p[5].hp > 0){
			    if( (p[2].hp = p[5].hp) > bai(p[2].type+6) )
				p[2].hp = bai(p[2].type+6);
			    p[5].hp = 0;
			    p[5].x = 1000;
			}
                    }
		    if(game.co[1] == -1){
		 	game.co[1] = -5;
			p[2].ap = 0;
			hst[2] = 0;
		    }
                    if(game.co[0] != 5){
                        if( (l = sqrt(speed.x*speed.x + speed.y*speed.y)) < 7){
                            k = M_PI * (10 + rand()%80)/100;
                            speed.y = l * sin(k) * bai(p[2].type);
                            speed.x = l * cos(k) * bai(p[2].type);
                        }else{
                            k = M_PI * (10 + rand()%80)/100;
                            speed.y = l * sin(k);
                            speed.x = l * cos(k);
                        }
                        game.defe[0][1] = def_ugoki(-1);
                        if(game.co[1] == 10){
			    p[2].ap = 0;
                            game.spd[0] = speed.x;
                            game.spd[1] = speed.y;
                            speed.x = 0;
                            speed.y = game.co[1];
                            game.defe[0][1] = pad.x;        
                        }
                        game.defe[1][1] = 0;
                        def_ugoki2();
                    }else{
                        if((p[0].hp += 160) > bai(p[0].type + 6))
                            p[0].hp = bai(p[0].type + 6);
                        game.co[0] = 0;
			hst[0] = 0;
		    }
                }   
            }else if(-pad.y + PAD_R >= SUP_Y && -pad.y + PAD_R <= SUP_Y - speed.y){
                if(pad.x+PAD_R > p[3].x - SUP_W && pad.x-PAD_R < p[3].x + SUP_W){
		    sound_flag = 1;
    		    if(game.co[0] == -10 && speed.y == game.co[0]){//必殺
	    		game.co[0] = 0;
			hst[0] = 0;
			p[3].hp -= 25*game.han[1];
			speed.x = game.spd[0];
	    		speed.y = game.spd[1];
    		    }
		    if( (p[3].ap += sqrt(speed.x*speed.x + speed.y * speed.y)) > 100)
			p[3].ap = 100;
                    if( (p[3].hp -= sqrt(speed.x*speed.x + speed.y * speed.y)*game.han[1]) <= 0 ){      //ｈｐ減少
			if(p[5].hp <= 0){
                            p[3].x = 1000;//HPが0以下になった時の処理
			    p[3].hp = 0;
			}
			if(p[5].hp > 0){
			    if( (p[3].hp = p[5].hp) > bai(p[i].type+6) )
				p[3].hp = bai(p[i].type+6);
			    p[5].hp = 0;
			    p[5].x = 1000;
			}
                    }
                    if(game.co[0] != 5){
                        if(sqrt(speed.x*speed.x + speed.y * speed.y) > 3){ 
                            speed.y = -speed.y * bai(p[3].type);
                            speed.x = speed.x * bai(p[3].type);
                        }else
                            speed.y = -speed.y;
                        game.defe[0][1] = def_ugoki(-1);
		  	game.defe[1][1] = 0;
			def_ugoki2();
                        if(p[2].hp > 0)                    //ｈｐ回復
                            p[2].hp += bai(p[3].type+2);
                        if(p[2].hp > bai(p[2].type + 6))
                            p[2].hp = bai(p[2].type + 6);
                        if(p[5].hp > 0)
                            p[5].hp += bai(p[3].type+2);
                        if(p[5].hp > bai(p[5].type + 6))
                            p[5].hp = bai(p[5].type + 6);
                    }else{
                        if((p[0].hp += 160) > bai(p[0].type + 6))
                            p[0].hp = bai(p[0].type + 6);
                        game.co[0] = 0;
			hst[0] = 0;
		    }
                }
            }else if(-pad.y + PAD_R >= DEF_Y){
		if(-pad.y + PAD_R <= DEF_Y - speed.y && pad.x+PAD_R > p[5].x - DEF_W && pad.x-PAD_R < p[5].x + DEF_W){
    		    if(game.co[0] == -10 && speed.y == game.co[0]){//必殺
	    		game.co[0] = 0;
			hst[0] = 0;
			p[5].hp -= 25*game.han[1];
			speed.x = game.spd[0];
	    		speed.y = game.spd[1];
    		    }
                    if( (p[5].hp -= sqrt(speed.x*speed.x + speed.y * speed.y)*game.han[1]) <= 0 ){      //ｈｐ減少
                        p[5].x = 1000;//HPが0以下になった時の処理
	            p[5].hp = 0;
                    }
                    if(game.co[0] != 5){
                        if(sqrt(speed.x*speed.x + speed.y * speed.y) > 3){ 
                           speed.y = -speed.y * 0.9;
                            speed.x = speed.x * 0.9;
                        }else
                           speed.y = -speed.y;
                        game.defe[0][1] = def_ugoki(-1);
                        game.defe[1][1] = 0;
                        def_ugoki2();
                        sound_flag = 1;
                    }else{
                        if((p[0].hp += 160) > bai(p[0].type + 6))
                            p[0].hp = bai(p[0].type + 6);
                        game.co[0] = 0;
			hst[0] = 0;
		    }
		}
            }
        }

        //縦の壁にぶつかった時
	if(game.point[2] != 0 && game.point[2] != 2 && game.point[2] != 1){
        if(pad.y + PAD_R > FIELD_H){
            if(pad.x >= GOAL_W || pad.x <= -GOAL_W){
		sound_flag = 1;
		if(game.co[1] == 10 && speed.y == game.co[1]){
	    	    game.co[1] = 0;
		    hst[2] = 0;
		    speed.x = game.spd[0];
	    	    speed.y = game.spd[1];
		}
                pad.y = 2 * FIELD_H - pad.y - 2*PAD_R;
		speed.y = -speed.y;
                game.defe[1][1] = def_ugoki(1);
	        game.defe[0][1] = 0;
	        def_ugoki2();
            }else{
                win = 1;
		game.point[2] = 0;
                if(++game.point[1] == max_point)//試合勝利条件
		    game.point[2] = 1;
                fprintf(stderr,"x[%d] - [%d]o\n",game.point[0],game.point[1]);
            }
        }else if(pad.y - PAD_R <= (-1)*FIELD_H){
            if(pad.x >= GOAL_W || pad.x <= -GOAL_W){
		sound_flag = 1;
		if(game.co[0] == -10 && speed.y == game.co[0]){
	    	    game.co[0] = 0;
		    hst[0] = 0;
		    speed.x = game.spd[0];
	    	    speed.y = game.spd[1];
		}
                pad.y = (-2) * FIELD_H - pad.y + 2*PAD_R;
		speed.y = -speed.y;
                game.defe[0][1] = def_ugoki(-1);
	        game.defe[1][1] = 0;
	        def_ugoki2();
            }else{
                win = 0;
		game.point[2] = 2;
                if(++game.point[0] == max_point)//試合勝利条件
		    game.point[2] = 1;
                fprintf(stderr,"o[%d] - [%d]x\n",game.point[0],game.point[1]);
            }
        }
	}else if(pad.y > FIELD_H + 30 || pad.y < -FIELD_H - 30){
            game.time = game.now;
	    game.scene = 1;
	}
    }else{
	if(game.scene == 3){
	    shokika();
	    game.scene = 2;
	    game.point[2] = 1;
	    game.time = game.now-2;
	}
	if(game.scene == 1 && game.now - game.time > 2){
	    game.scene = 2;
	    if(game.point[2] == 0)
	        pad.y = ATK_Y-PAD_R-3;
	    if(game.point[2] == 2)
	        pad.y = -ATK_Y+PAD_R+3;
	    if(game.point[2] == 1){
		pad.y = 0;
		pad.x = 0;
	    }
	    shokika();
	}
	if(game.now - game.time > 2)
	    if(game.point[2] != 1)
	    	pad.x = p[game.point[2]].x;
	    else if(game.now - game.time > 5){
		game.point[2] = -1;
                k = M_PI * (10 + rand()%80)/100;
                speed.y = 3 * sin(k);
                speed.x = 3 * cos(k);
		if(rand()%2 == 0)
		    speed.y = -speed.y;
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
		game.scene = 0;
	    }
	if(game.now - game.time > 3 && start_flag && game.point[2] != 1){
	    game.scene = 0;
	    if(game.point[2] == 0)
	        speed.y = 3;
	    if(game.point[2] == 2)
		speed.y = -3;
	    p[game.point[2]].hp += speed.y*10;
	    game.point[2] = -1;
	}
	start_flag = 0;
    }
    /*
    fprintf(stderr, "***********************************\n");
    for (i = 0; i < 6; i++) {
        fprintf(stderr, "    p%d  x:%f\n", i, p[i].x);
    }
    fprintf(stderr, "    PAD x:%f y:%f\n", pad.x, pad.y);
    fprintf(stderr, "***********************************\n");
    */
    current_frame++;
}
