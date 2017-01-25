/*************************************
  common.h
  サーバ・クライアント共通ヘッダファイル
  徳島大学 工学部 知能情報工学科 27班
*************************************/

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>

/* ネットワーク */
#define DEFAULT_PORT   51000
#define MAX_LEN_BUFFER 256
#define MAX_LEN_ADDR   32
#define BROADCAST      -1

/* コマンド */
#define COM_NONE       'N'    // None      何もなし（座標をやり取りする）
#define COM_SPECIAL    'S'    // Special   必殺技
#define COM_CONTINUE   'C'    // Continue  コンティニュー
#define COM_EXIT       'X'    // eXit      終了
#define COM_WIN        'W'    // Win       「Win」表示   (点数送信)
#define COM_LOSE       'L'    // Lose      「Lose」表示  (点数送信)
#define COM_READY      'R'    // Ready     「Ready」表示 
#define COM_START      'T'    // sTart     「Start」表示

/* フィールド */
#define FIELD_H  160          // フィールドの縦幅
#define FIELD_W  100          // フィールドの横幅
#define GOAL_W   30           // ゴールの幅

/* プレイヤー */
#define DEF_Y 150             // ディフェンダーのY座標
#define DEF_W 30              // ディフェンダーの幅
#define SUP_Y 115             // サポーターのY座標
#define SUP_W 10              // サポーターの幅
#define ATK_Y 75              // アタッカーのY座標
#define ATK_W 20              // アタッカーの幅

/* パッド */
#define PAD_R 10              // パッドの半径

/* クライアント情報用の構造体 */
typedef struct{
    int cid;
    int sock;
    int control;
    struct sockaddr_in addr;
}CLIENT;

/* プレイヤーを表す構造体 */
typedef struct{
    int   type;               // プレイヤーの種類(0,1:アタッカー  2,3:サポーター  4:ディフェンダー）0マクリー,1リーパー　2ゼニヤッタ,3マーシー
    int   hp;                 // プレイヤーの体力
    int   ap;                 // 必殺技ポイント
    float x;                  // プレイヤーのX座標
}PLAYER;

/* パッドの構造体 */
typedef struct{
    float x;                  // パッドのX座標
    float y;                  // パッドのY座標
}PAD;

/* サーバ→クライアント用の構造体 */
typedef struct{
    char   com;               // コマンド
    int    frame;             // フレーム番号
    PAD    pad;               // パックの座標
    PLAYER p[6];              // プレイヤー情報
}CONTAINER_S;

/* クライアント→サーバ用の構造体 */
typedef struct{
    char  com;                // コマンド
    int   frame;              // フレーム番号
    float x;                  // プレイヤーのX座標
}CONTAINER_C;
