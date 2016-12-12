/*************************************
  client_main.c
  徳島大学 工学部 知能情報工学科 27班
*************************************/

#include "client.h"
#include <SDL/SDL.h>
#include <time.h>
#include <sys/time.h>

float gt;
float ti;

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
        SDL_Delay(5);
    }

    SDL_WaitThread(thr1, NULL);
    terminate_client();    

    return 0;

#else
    init_sdl();
    //時間	
    time_t timer;
    struct tm *now;
    struct timeval mt;	

/* 現在時刻の取得 */
    time(&timer);
    now = localtime(&timer);
    gt = now->tm_sec + now->tm_hour * 3600 + now->tm_min * 60 + mt.tv_usec * 0.1;

    while (flag) {
	time(&timer);
   	now = localtime(&timer);
	ti = now->tm_sec + now->tm_hour * 3600 + now->tm_min * 60 + mt.tv_usec * 0.1;
	if(10*(ti-gt)>1){        
	    field_set();
	    gt = now->tm_sec + now->tm_hour * 3600 + now->tm_min * 60 + mt.tv_usec * 0.1;
	}
        flag = Keyevent();
        draw_field();
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
    return 0;
}
