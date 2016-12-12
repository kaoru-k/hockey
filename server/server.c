/*************************************
  server.c
  サーバのメインモジュール
  徳島大学 工学部 知能情報工学科 27班
*************************************/

#include "server.h"
#include <SDL/SDL.h>
#include <time.h>

#include <sys/time.h>
int flag = 1;
PAD pad = {1,1};
float gt;
float ti;

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

//時間	
    time_t timer;
    struct tm *now;
    struct timeval mt;	

/* 現在時刻の取得 */
    time(&timer);
    now = localtime(&timer);
    gt = now->tm_sec + now->tm_hour * 3600 + now->tm_min * 60 + mt.tv_usec * 0.1;

    while(flag) {
	ti = now->tm_sec + now->tm_hour * 3600 + now->tm_min * 60 + mt.tv_usec * 0.1;
	if(10*(ti - gt) > 1)
	field_set();
        //SDL_Delay(10);
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
