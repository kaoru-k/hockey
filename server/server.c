/*************************************
  server.c
  サーバのメインモジュール
  徳島大学 工学部 知能情報工学科 27班
*************************************/

#include "server.h"
#include <SDL/SDL.h>

int flag = 1;
PAD pad = {1,1};

static int network_thread(void* args);

int main(int argc, char *argv[])
{
    u_short port = DEFAULT_PORT;
    SDL_Thread *thr1;

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
    thr1 = SDL_CreateThread(network_thread, NULL);

    while(flag) {
	field_set();
    }
    SDL_WaitThread(thr1, NULL);
    terminate_server();

    return 0;
}

static int network_thread(void* args)
{
    fprintf(stderr, "network_thread started.\n");
    while (flag) {
        flag = network();
    }
    return 0;
}
