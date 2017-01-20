/*************************************
  client_game.c
  クライアントのゲームモジュール
  徳島大学 工学部 知能情報工学科 27班
*************************************/

#include "client.h"
#include <SDL/SDL.h>

float speedx[2] = {0.0, 0.0};	// 自分の操作速度
SDL_Joystick *joystick;         // ジョイスティックを特定・利用するための構造体
int endflag = 0;
int reset = 0;

PLAYER p[6] = {{0,650,0,0},
               {2,300,0,0},
               {1,550,0,0},
               {3,450,0,0},
               {4,800,0,0},
               {4,800,0,0}};

int Keyevent(void)
{   
    // イベントを処理する
    SDL_Event event;
    if(SDL_PollEvent(&event)) {
        switch (event.type){
        case SDL_QUIT :
            endflag = 1;
#ifdef TEST            
            return 0;
#else
            return 1;
#endif
        case SDL_KEYDOWN :
            switch(event.key.keysym.sym){
            case SDLK_ESCAPE:
                endflag = 1;
#ifdef TEST            
                return 0;
#else
                return 1;
#endif              
            case SDLK_RIGHT:
                camera.x -= 5;
                break;
            case SDLK_LEFT:
                camera.x += 5;
                break;
            case SDLK_UP:
                camera.y -= 5;
                break;
            case SDLK_DOWN:
                camera.y += 5;
                break;
            case SDLK_d:
			speedx[0] = -3;
                break;
            case SDLK_a:
			speedx[0] = 3;
                break;
            case SDLK_w:
            case SDLK_s:
                speedx[0] = 0;
                break;
	    case SDLK_n:
		cameramode = 0;
		camera.x = 0;
		camera.y = 140;
                break;
            case SDLK_m:
		cameramode = 1;
                camera.x = 0;
                camera.y = -140;
                break;
            default:
                break;
            }
        case SDL_JOYAXISMOTION:
            //printf("The axis ID of the operated key is %d.\n",event.jaxis.axis);
                // 操作された方向キーの方向軸を表示（0：アナログキー，1：アナログキー，2：方向キー左右方向，3：方向キー上下方向）
            if(event.jaxis.axis==0){
                //printf("--- Analog-Direction Key: ?? Axis\n");
                if(event.jaxis.value < 0){
                    if(myid == 0 || myid == 1)
                	speedx[0] = 3;
		    else if(myid == 2 || myid == 3)
			speedx[0] = -3;
                }else if(event.jaxis.value >  0){
                    if(myid == 0 || myid == 1)
                	speedx[0] = -3;
		    else if(myid == 2 || myid == 3)
			speedx[0] = 3;
                }else
                    speedx[0] = 0.0;
            }
        }
	
    }
    
    network_send();
    if(FIELD_W > p[myid].x + speedx[0] + ATK_W && p[myid].x + speedx[0] - ATK_W > -FIELD_W)
        p[myid].x += speedx[0];
    
    return 1;   
}
