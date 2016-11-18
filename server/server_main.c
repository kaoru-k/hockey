/*************************************
  server_main.c
  サーバのメインモジュール
  徳島大学 工学部 知能情報工学科 27班
*************************************/

#include "server.h"

int main(int argc, char *argv[])
{
    u_short port = DEFAULT_PORT;
    int flag = 1;

    switch(argc) {
    case 1:
        break;
    case 2:
        port = atoi(argv[1]);
        break;
    default:
        fprintf(stderr, "Too many argument!\n");
        return 1;
    }

    fprintf(stderr, "Port number = %d\n", port);

    setup_server(port);

    while(flag) {
    }

    terminate_server();

    return 0;
}
