/*************************************
  client.h
  クライアントのヘッダファイル
  徳島大学 工学部 知能情報工学科 27班
*************************************/

#include "../common.h"
#include <SDL/SDL.h>

/***** 定数 *****/


/***** グローバル変数 *****/
extern PLAYER p[6]; //0,1,2,3->human 4,5->cpu
extern PAD pad;
extern int myid;
extern SDL_Rect pack;
extern SDL_Rect camera;
extern SDL_Joystick *joystick;
extern int endflag;

/***** 外部関数 *****/
// client_net.c
extern void setup_client(char *server_name, u_short port);
extern void terminate_client(void);
extern int  network(void);

// client_game.c
extern int  Keyevent(void);

// client_graphic.c
extern int  init_sdl(void);
extern int  draw_field(void);

#ifdef TEST
// server_game.c (テスト用)
extern void field_set(void);
extern int  def_ugoki(int i);
#endif
