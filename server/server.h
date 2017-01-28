/*************************************
  server.h
  サーバ用ヘッダファイル
  徳島大学 工学部 知能情報工学科 27班
*************************************/
#include "../common.h"

/***ゲーム状態格納用構造体**/
typedef struct{
    int time;       //時間を計るため
    int now;        //現在の時間を知るため
    int heal;
    int scene;      //0:ゲーム画面　1:ゴールした瞬間 2:ゴール後球を構えている時
    int point[3];   //点数　point[0]:p[2]p[3]側　point[1]:p[0]p[1]側 point[2]:前回負けたチーム
    int defe[2][2]; //ディフェンダーの移動する向きと目標座標
    int co[2];      //こまんど　
    double spd[2];  //コマンドを発動した時に,スピードを一時的に保存するため
    double han[2];  //ダメージ倍率  基本10、ゼニヤッタの必殺時2倍(20)
}GAME;

/***** グローバル変数 *****/
extern int    num_clients;
extern CLIENT clients[4];
extern PLAYER p[6]; //0,1,2,3->human 4,5->cpu
extern PAD    pad;
extern GAME   game;
extern float  gt;
extern float  ti;
extern int    current_frame;
extern int    client_frame[4];
extern int    endflag;
extern int    start_flag;
extern int    sound_flag;
extern int    win;

/***** 外部関数 *****/
// server_net.c
void setup_server(u_short port);
void setting_server(void);
void terminate_server(void);
int  network(void);
int s_on(void);

// server_game.c
void field_set(void);
