/*************************************
  server_main.c
  サーバのメインモジュール
  徳島大学 工学部 知能情報工学科 27班
*************************************/

#include "server.h"
#include <SDL/SDL.h>

static Uint32 network_thread(void* args);

int main(int argc, char *argv[])
{
    u_short port = DEFAULT_PORT;
    int flag = 1;
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

    terminate_server();

    return 0;
}

static Uint32 network_thread(void* args)
{
    
}
