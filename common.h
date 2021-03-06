/*************************************
  common.h
  サーバ・クライアント共通ヘッダファイル
  徳島大学 工学部 知能情報工学科 27班
*************************************/

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>

/* ネットワーク */
#define DEFAULT_PORT   51000  // デフォルトのポート番号 
#define MAX_LEN_BUFFER 256
#define MAX_LEN_ADDR   32
#define BROADCAST      -1

/* コマンド */
#define COM_NONE       'N'    // None      何もなし（座標をやり取りする）
#define COM_SPECIAL    'S'    // Special   必殺技
#define COM_S_AND_B    'D'    // S anD B   必殺技+跳ね返る音
#define COM_START      'T'    // sTart     ゲームスタート
#define COM_LAUNCH     'A'    // lAunch    発射準備
#define COM_QUE_Y      'Y'    // Yusho     コンティニューするかどうか
#define COM_QUE_Z      'Z'    // Zanpai    同上
#define COM_RESET      'R'    // Reset     初期化
#define COM_ALLRESET   'E'    // allrEset  オールリセット
#define COM_EXIT       'X'    // eXit      終了
#define COM_WIN        'W'    // Win       「Win」表示   (点数送信)
#define COM_LOSE       'L'    // Lose      「Lose」表示  (点数送信) 
#define COM_BOUND      'B'    // Bound     跳ね返る音
#define COM_KILL       'K'    // Kill      死んだ時の音

/* フィールド */
#define FIELD_H        160    // フィールドの縦幅
#define FIELD_W        100    // フィールドの横幅
#define GOAL_W         30     // ゴールの幅

/* プレイヤー */
#define DEF_Y          150    // ディフェンダーのY座標
#define DEF_W          30     // ディフェンダーの幅
#define SUP_Y          115    // サポーターのY座標
#define SUP_W          10     // サポーターの幅
#define ATK_Y          75     // アタッカーのY座標
#define ATK_W          20     // アタッカーの幅

/* 各キャラの体力 */
#define DEF_HP0        800
#define ATK1_HP0       550
#define ATK2_HP0       650
#define SUP1_HP0       300
#define SUP2_HP0       400

/* パッド */
#define PAD_R          10     // パッドの半径

/* クライアント情報用の構造体 */
typedef struct{
    int cid;                  // プレイヤー番号
    int sock;                 // ソケット
    int control;              // 操作するキャラクター(0,3→アタッカー 1,4→サポーター)
    struct sockaddr_in addr;  // クライアントのアドレス
}CLIENT;

/* プレイヤーを表す構造体 */
typedef struct{
    int   cid;
    int   type;               // プレイヤーの種類(0,1:アタッカー  2,3:サポーター  4:ディフェンダー）0マクリー,1リーパー　2ゼニヤッタ,3マーシー
    int   hp;                 // プレイヤーの体力
    int   ap;                 // 必殺技ポイント
    float x;                  // プレイヤーのX座標
}PLAYER;

/* コンテナ用のプレイヤー構造体 */
typedef struct{
    int   hp;
    int   ap;
    float x;
}PLAYER2;

/* パッドの構造体 */
typedef struct{
    float x;                  // パッドのX座標
    float y;                  // パッドのY座標
}PAD;

/* 座標などをサーバからクライアントに送るときに使う構造体 */
typedef struct{
    char    com;               // コマンド
    int     frame;             // フレーム番号
    PAD     pad;               // パックの座標
    PLAYER2 p[6];              // プレイヤー情報
}CONTAINER_S;

/* 座標などをクライアントからサーバに送るときに使う構造体 */
typedef struct{
    char  com;                // コマンド
    int   frame;              // フレーム番号
    float x;                  // プレイヤーのX座標
}CONTAINER_C;

/* 設定をクライアントからサーバに送るときに使う構造体 */
typedef struct{
    int point;                // 何点先取で勝ちになるか
    int chara;                // 選んだキャラクター
}SETTING;

/* 設定をサーバからクライアントに送るときに使う構造体 */
typedef struct{
    int point;
    int type[6];
}SETTING2;
