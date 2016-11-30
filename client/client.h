/*************************************
  client.h
  クライアントのヘッダファイル
  徳島大学 工学部 知能情報工学科 27班
*************************************/

#include "../common.h"

/***** 定数 *****/

/***** グローバル変数 *****/
PLAYER p[6]; //0,1,2,3->human 4,5->cpu
PAD pad;

/***** 外部関数 *****/
// client_net.c
extern void setup_client(char *server_name, u_short port);
extern void terminate_client(void);

// client_game.c

// client_graphic.c
