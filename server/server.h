/*************************************
  server.h
  サーバ用ヘッダファイル
  徳島大学 工学部 知能情報工学科 27班
*************************************/
#include "../common.h"

/***** グローバル変数 *****/
extern int    num_clients;
extern CLIENT clients[4];
extern PLAYER p[6]; //0,1,2,3->human 4,5->cpu
extern PAD    pad;
extern int    game_scene;
extern float  gt;
extern float  ti;
extern int    current_frame;
extern int    client_frame[4];

/***** 外部関数 *****/
// server_net.c
void setup_server(u_short port);
void terminate_server(void);
int  network(void);

// server_game.c
void field_set(void);
