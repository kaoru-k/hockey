/*************************************
  client.c
  クライアントのメインモジュール
  徳島大学 工学部 知能情報工学科 27班
*************************************/

#include "client.h"
#include <SDL/SDL.h>
#include <time.h>
#include <sys/time.h>

int flag = 1;
int endflag = 0;
int current_frame = 0;

static int network_thread(void* args);
static int game_thread(void *args);

int main(int argc, char *argv[])
{  
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
    setup_sound();

    int test = 0;
    SDL_Event event;
    
    while(endflag == 0) {
        
	while(flag){//スタート画面
            StartWindow();
            if(SDL_PollEvent(&event)){
		if(event.type == SDL_JOYBUTTONDOWN)
		    flag = 0;
	    }
        }
        flag = 1;
	setting.point = 3;
        while(flag){//設定画面
            SettingWindow();
            Keyevent();
            if(myid == 0 && settingflag == 3)
		flag = 0;
            if(myid != 0 && settingflag == 2)
		flag = 0;
        }

        flag = 1;
        setting_client();
        while(flag){//設定画面
            WaitingWindow();
            if(SDL_PollEvent(&event)){
		if(event.type == SDL_JOYBUTTONDOWN)
		    flag = 0;
	    }
        }      
	setting_done();
        flag = 1;

        draw_field();
        if(myid == 0 || myid == 1){
            cameramode = 0;
            camera.x = 0; camera.y = 140;
        }
        else
        {
            cameramode = 1;
            camera.x = 0; camera.y = -140;
        }

        thr1 = SDL_CreateThread(network_thread, &flag);

        while (flag) {
            flag = Keyevent();
            network_send();
            if (current_frame % 2) draw_field();
            current_frame++;
        }
        SDL_WaitThread(thr1, NULL);
    }
    terminate_client();
    return 0;
}
    
static int network_thread(void* args)
{
    fprintf(stderr, "network_thread() started.\n");
    while (flag) {
        flag = network_recv();
    }
    return 0;
}
