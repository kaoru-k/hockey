/*************************************
  client_game.c
  クライアントのゲームモジュール
  徳島大学 工学部 知能情報工学科 27班
*************************************/

#include "client.h"
#include <SDL/SDL.h>

float speedx[2] = {0.0, 0.0};	// 自分の操作速度
SDL_Joystick *joystick;         // ジョイスティックを特定・利用するための構造体
int reset = 0;
int setting[2] = {0, 1};
int settingflag = 0;


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
            return 1;
        case SDL_KEYDOWN :
            switch(event.key.keysym.sym){
            case SDLK_ESCAPE:
                endflag = 1;
                return 1;
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
            break;
        case SDL_JOYAXISMOTION:
            if(event.jaxis.axis==0){
                if(event.jaxis.value < 0){
                    if((settingflag == 0 && 0 < setting[settingflag]) || settingflag == 1 &&  1 < setting[settingflag])
			setting[settingflag]--; 

                    if(control_id == 0 || control_id == 1)
                	speedx[0] = 3;
		    else if(control_id == 2 || control_id == 3)
			speedx[0] = -3;
                }else if(event.jaxis.value >  0){
                    if((settingflag == 0 && setting[settingflag] < 3) || (settingflag == 1 && setting[settingflag] < 10))
		        setting[settingflag]++; 
                    if(control_id == 0 || control_id == 1)
                	speedx[0] = -3;
		    else if(control_id == 2 || control_id == 3)
			speedx[0] = 3;
                }else
                    speedx[0] = 0.0;
            }
	    if(event.jaxis.axis==1){
		if(event.jaxis.value > 0){
		    if(settingflag == 0) settingflag = 1;
		}else if(event.jaxis.value < 0){
		    if(settingflag == 1) settingflag = 0;
		}
	    }
            break;
        case SDL_JOYBUTTONDOWN:
            switch(event.jbutton.button){
            case 0 :
                send_flag = COM_START;   break;
            case 1 :
                send_flag = COM_SPECIAL; break;
            case 4 :
                camera.x += 5; break;
            case 5 :
                camera.x -= 5; break;
            case 11:
                endflag = 1; break;
            }
	    if(settingflag == 0 || settingflag == 1)
		settingflag = 3;
            break;
	default:
            send_flag = COM_NONE; break;
        }
    }

    if (p[control_id].hp > 0 && (control_id == 0 ||control_id == 2)) {
        if(FIELD_W > p[control_id].x + speedx[0] + ATK_W && p[control_id].x + speedx[0] - ATK_W > - FIELD_W)
            p[control_id].x += speedx[0];
        if(FIELD_W < p[control_id].x + ATK_W || p[control_id].x - ATK_W < - FIELD_W)
            p[control_id].x = 0;
    }

    if (p[control_id].hp > 0 && (control_id == 1 ||control_id == 3)) {
        if(FIELD_W > p[control_id].x + speedx[0] + SUP_W && p[control_id].x + speedx[0] - SUP_W > - FIELD_W)
            p[control_id].x += speedx[0];
        if(FIELD_W < p[control_id].x + SUP_W || p[control_id].x - SUP_W < - FIELD_W)
            p[control_id].x = 0;
    }
    return 1;   
}
