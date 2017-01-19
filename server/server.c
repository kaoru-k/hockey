/*************************************
  server.c
  サーバのメインモジュール
  徳島大学 工学部 知能情報工学科 27班
*************************************/

#include "server.h"
#include <SDL/SDL.h>
#include <time.h>
#include <sys/time.h>

int num_clients = 4;
int flag = 1;
PAD pad = {1,1};

static int network_thread(void* args);

int main(int argc, char *argv[])
{
    u_short port = DEFAULT_PORT;
    
    SDL_Thread *thr1;
    double time_now = 0;
    double time_last = 0;
    struct timespec time_tmp;

    switch(argc) {
    case 1:
        break;
    case 2:
        num_clients = atoi(argv[1]);
        break;
    case 3:
        num_clients = atoi(argv[2]);
        port = atoi(argv[2]);
        break;
    default:
        fprintf(stderr, "Too many argument!\n");
        return 1;
    }

    if (num_clients > 4) num_clients = 4;
    fprintf(stderr, "Number of clients = %d\n", num_clients);
    fprintf(stderr, "Port number = %d\n", port);

    setup_server(port);
    thr1 = SDL_CreateThread(network_thread, NULL);

    while(flag) {
	clock_gettime(CLOCK_REALTIME, &time_tmp);
        time_now = (int)time_tmp.tv_sec + (double)time_tmp.tv_nsec * 0.000000001;
	if(time_now - time_last > 0.016){
	    field_set();
	    time_last = time_now;
	}
    }

    SDL_WaitThread(thr1, NULL);

    return 0;
}

static int network_thread(void* args)
{
    fprintf(stderr, "network_thread started.\n");
    while (flag) {
        flag = network();
    }
    terminate_server();
    return 0;
}
