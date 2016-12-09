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

PLAYER p[6] = {{0,65,0,0},
               {2,30,0,0},
               {1,55,0,0},
               {3,45,0,0},
               {4,80,0,0},
               {4,80,0,0}};

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
                camera.x -= 1;
                break;
            case SDLK_LEFT:
                camera.x += 1;
                break;
            case SDLK_UP:
                camera.y -= 1;
                break;
            case SDLK_DOWN:
                camera.y += 1;
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
            default:
                break;
            }
        case SDL_JOYAXISMOTION:
            printf("The axis ID of the operated key is %d.\n",event.jaxis.axis);	// 操作された方向キーの方向軸を表示（0：アナログキー，1：アナログキー，2：方向キー左右方向，3：方向キー上下方向）
            if(event.jaxis.axis==0){
                printf("--- Analog-Direction Key: ?? Axis\n");
                if(event.jaxis.value < 0)
                    speedx[1] += 3;
                else if(event.jaxis.value >  0)
                    speedx[1] -= 3;
                else
                    speedx[1] = 0.0;
            }
            else if(event.jaxis.axis==1){
                printf("--- Anolag-Direction Key: ?? Axis\n");
                /*else
                  cursor_axis.y = 0;*/
            }
            else if(event.jaxis.axis==2){
                printf("--- Four-Direction Key: Horizontal Axis\n");
		
            }
            else if(event.jaxis.axis==3){
                printf("--- Four-Direction Key: Vertical Axis\n");
            }						
            printf("--- The axis value of the operated key is %d.\n",event.jaxis.value);	// ジョイスティックの操作された方向キーの値を表示（-32767（真左，真上）～32767（真右，真下））
            break;
            // ジョイスティックのボタンが押された時
            /*case SDL_JOYBUTTONDOWN:
              switch(event.jbutton.button){
              case 4:
              case 6: if(cursormode == 1)
              cursormode = 0;
              break;
              case 5:
              case 7: if(cursormode == 0)
              cursormode = 1;
              break;
              case 0 :
              case 1 :
              case 2 :
              case 3 :	if(prog == 0){
              prog = 1;
              textmode = 1;
              }
              
              if(clear == 1){
              prog++;
              textmode = 1;
              clear = 0;
              }
              
              if(chara_rect.y + 90 == GROUND){
              if(chara_rect.x <= cursor_rect.x + 25 && cursor_rect.x + 25 <= chara_rect.x + 92 && chara_rect.y +  90 <= cursor_rect.y + 25 && cursor_rect.y + 25 <= chara_rect.y +  90 + 25 && cursormode == 0)
              accely = 8.0;
              if(chara_rect.x <= cursor_rect.x + 25 && cursor_rect.x + 25 <= chara_rect.x + 92 && chara_rect.y <= cursor_rect.y + 25 && cursor_rect.y + 25 <= chara_rect.y +  90  && cursormode == 0)
              SDL_Delay(1000);
              }	 
              break;
              case 10 :
              case 11 : return 0;
              break;
              default : break;
              }
              printf("The ID of the pressed button is %d.\n", event.jbutton.button);	// 押されたボタンのIDを表示（0から）
              // ボタンIDに応じた処理
              if(event.jbutton.button==0){
              printf("--- You pressed a button on the joystick.\n");
              }
              break;
              // ボタンが離された時
              case SDL_JOYBUTTONUP:
              printf("The ID of the released button is %d.\n", event.jbutton.button);	// 離されたボタンのIDを表示（0から）
              // ボタンIDに応じた処理
              if(event.jbutton.button==0){
              printf("--- You released a button on the joystick.\n");
              }
              break;*/
        }
	
    }

    if(FIELD_W > p[myid].x + speedx[0] + ATK_W && p[myid].x + speedx[0] - ATK_W > -FIELD_W)
        p[myid].x += speedx[0];
//    if(FIELD_W > p[1].x + speedx[1] + ATK_W && p[1].x + speedx[1] - ATK_W > -FIELD_W)
//        p[1].x += speedx[1];
    
    return 1;   
}
