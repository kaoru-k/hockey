/*************************************
  client.h
  クライアントのヘッダファイル
  徳島大学 工学部 知能情報工学科 27班
*************************************/

#include "../common.h"
#include <SDL/SDL.h>

/***** 定数 *****/
#define M_BOUND 0

/***** グローバル変数 *****/
extern PLAYER p[6];           //0,1,2,3->human 4,5->cpu
extern PAD  pad;
extern int  myid;
extern int  control_id;
extern int  cameramode;
extern int  current_frame;
extern int  flag;
extern int  endflag;
extern char send_flag;
extern int  recv_flag;
extern int  point[2];
extern SDL_Rect     pack;
extern SDL_Rect     camera;
extern SDL_Joystick *joystick;
extern int setting[2];
extern int settingflag;

/***** 外部関数 *****/
// client_net.c
extern void setup_client(char *server_name, u_short port);
extern void setting_client(void);
extern void terminate_client(void);
extern int  network_send(void);
extern int  network_recv(void);

// client_game.c
extern int  game_setting(void);
extern int  Keyevent(void);

// client_graphic.c
extern int  init_sdl(void);
extern int  draw_field(void);
extern void  StartWindow(void);
extern void  SettingWindow(void);

// client_sound.c
extern int  setup_sound(void);
extern void play_sound(int index);
