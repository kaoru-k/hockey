#include "../common.h"

/***** 定数 *****/

/***** グローバル変数 *****/
PLAYER p[6]; //0,1,2,3->human 4,5->cpu
PAD pad;

/***** 外部関数 *****/
// client_net.c
void client_server(char *server_name, u_short port);
void recv_data(void *data, int size);
void send_data(void *data, int size);
void terminate_client(void);

// client_game.c

// client_ui.c
