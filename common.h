/************************************
  server・client共通ヘッダファイル
  徳島大学 工学部 知能情報工学科 27班 
*************************************/
#include <netinet/in.h>

/* フィールド */
#define FIELD_H  400   //フィールドの縦幅
#define FIELD_W  300   //フィールドの横幅
#define GOAL_W   100   //ゴールの幅

/* プレイヤー */
#define DEF_Y 300      //ディフェンダーのY座標
#define DEF_W 30       //ディフェンダーの幅
#define SUP_Y 200      //サポーターのY座標
#define SUP_W 20       //サポーターの幅
#define ATK_Y 100      //アタッカーのY座標
#define ATK_W 10       //アタッカーの幅

/* パッド */
#define PAD_R 10       //パッドの半径


/* プレイヤーを表す構造体 */
typedef struct{
    int   type;    // プレイヤーの種類(0:アタッカー,1:サポーター,2:ディフェンダー)
    int   hp;      // プレイヤーの体力
    int   ap;      // 必殺技ポイント
    float x;       // プレイヤーのX座標
}PLAYER;

/* パッドの構造体 */
typedef struct{
    float speed_x;   // パッドのX軸方向速度
    float speed_y;   // パッドのY軸方向速度
    float x;         // パッドのX座標
    float y;         // パッドのY座標
}PAD;
