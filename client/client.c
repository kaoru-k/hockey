/*************************************
  client_main.c
  徳島大学 工学部 知能情報工学科 27班
*************************************/

#include "client.h"
#include <SDL/SDL.h>
#ifdef TEST
#include <time.h>
#include <sys/time.h>
#endif

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
    thr1 = SDL_CreateThread(network_thread, &flag);

    while (flag) {
        flag = Keyevent();
        draw_field();
    }

    SDL_WaitThread(thr1, NULL);   

    return 0;

#else
    
    double time_now = 0;
    double time_last = 0;
    struct timespec time_tmp;
    init_sdl();
    
    while (flag) {
	clock_gettime(CLOCK_REALTIME, &time_tmp);
        time_now = (int)time_tmp.tv_sec + (double)time_tmp.tv_nsec * 0.000000001;
	if(time_now - time_last > 0.016){
            field_set();
            time_last = time_now;
	}
        flag = Keyevent();
        draw_field();
        fprintf(stderr, "***********************************\nx:%f y:%f\n***********************************\n", pad.x, pad.y);
        SDL_Delay(5);
    }

    return 0;
#endif
}
    
static int network_thread(void* args)
{
    fprintf(stderr, "network_thread() started.\n");
    while (flag) {
        flag = network(); 
    }
    terminate_client();
    return 0;
}
