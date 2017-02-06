/*************************************
  client_sound.c
  クライアントのサウンド処理
  徳島大学 工学部 知能情報工学科 27班
*************************************/
#include "client.h"
#include <SDL/SDL_mixer.h>

static Mix_Music *bgm;
static Mix_Chunk *bound_sound;

static char bgm_file[]   = "";
static char bound_file[] = "audio/c.wav";

int setup_sound(void)
{
    if ( (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024)) < 0 )
        return 0;

    //bgm = Mix_LoadMUS( bgmfile );
    bound_sound = Mix_LoadWAV( bound_file );

    return 1;
}

void play_sound(int index)
{
    switch(index) {
    case M_BOUND :
        Mix_PlayChannel(-1, bound_sound, 0); break;
    }
}
