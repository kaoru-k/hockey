/*************************************
  server.h
  サーバ用ヘッダファイル
  徳島大学 工学部 知能情報工学科 27班
*************************************/
#include "../common.h"

/***** グローバル変数 *****/
CLIENT clients[4];
PLAYER p[6]; //0,1,2,3->human 4,5->cpu
PAD pad;

/***** 外部関数 *****/
// server_net.c
void setup_server(u_short port);
void terminate_server(void);
void network_test(void);

// server_game.c
void field_set(void);
