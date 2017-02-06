/*************************************
  client_sound.c
  クライアントのサウンド処理
  徳島大学 工学部 知能情報工学科 27班
*************************************/
#include "client.h"
#include <SDL/SDL_mixer.h>

static Mix_Music *bgm;
static Mix_Chunk *bound_sound;
static Mix_Chunk *atk1_sound;
static Mix_Chunk *atk2_sound;
static Mix_Chunk *atk22_sound;
static Mix_Chunk *c_sound;
static Mix_Chunk *sup1_sound;
static Mix_Chunk *sup2_sound;



static char bgm_file[]   = "";
<<<<<<< HEAD
static char bound_file[] = "audio/c.wav";
=======
static char c[]     = "audio/c.wav";
static char atk1[]  = "audio/atk1.wav";
static char atk2[]  = "audio/atk2.wav";
static char atk22[] = "audio/atk22.wav";
static char bound[] = "audio/bound.wav";
static char sup1[]  = "audio/sup1.wav";
static char sup2[]  = "audio/sup2.wav";


>>>>>>> 719efa417c0465285787098edf7c9ae8014396d9

int setup_sound(void)
{
    if ( (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024)) < 0 )
        return 0;

    //bgm = Mix_LoadMUS( bgmfile );
    c_sound      = Mix_LoadWAV( c );
    atk1_sound   = Mix_LoadWAV( atk1 );
    atk2_sound   = Mix_LoadWAV( atk2 );
    atk22_sound  = Mix_LoadWAV( atk22 );
    bound_sound  = Mix_LoadWAV( bound );
    sup1_sound   = Mix_LoadWAV( sup1 );
    sup2_sound   = Mix_LoadWAV( sup2 );
   

    return 1;
}

void play_sound(int index)
{
    switch(index) {
    case M_BOUND :
        Mix_PlayChannel(-1, bound_sound, 0); break;
    }
}
