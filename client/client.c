/*************************************
  client_main.c
  徳島大学 工学部 知能情報工学科 27班
*************************************/

#include "client.h"
#include <SDL/SDL.h>

int flag = 1;

static int network_thread(void* args);

int main(int argc, char *argv[])
{
#ifndef TEST   
    u_short port = DEFAULT_PORT;
    char server_name[50];
    SDL_Thread *thr1;

    sprintf(server_name, "localhost");

    switch(argc) {
    case 1:
        break;
    case 2:
        sprintf(server_name, "%s", argv[1]);
        break;
    case 3:
        sprintf(server_name, "%s", argv[1]);
        break;
    default:
        fprintf(stderr, "Too many argument!\n");
        return 1;
    }
    
    setup_client(server_name, port);
    init_sdl();
    thr1 = SDL_CreateThread(network_thread, NULL);

    while (flag) {
        flag = Keyevent();
        draw_field();
    }

    terminate_client();
    return 0;

#else
    init_sdl();
    
    while (flag) {
        field_set();
        flag = Keyevent();
        draw_field();
    }

    return 0;
#endif
}
    
static int network_thread(void* args)
{
    fprintf(stderr, "network_thread() started.\n");
    while (1) {
        network_test();
    }
    return 0;
}
