#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <GL/glut.h>
/* フィールド */
#define FIELD_H  160   //フィールドの縦幅
#define FIELD_W  100   //フィールドの横幅
#define GOAL_W   0   //ゴールの幅

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

PLAYER p[6] = {{  },{ },{ },{ },{ },{ }};

/* パッドの構造体 */
typedef struct{
    float speed_x;   // パッドのX軸方向速度
    float speed_y;   // パッドのY軸方向速度
    float x;         // パッドのX座標
    float y;         // パッドのY座標
}PAD;
PAD pad={2,2,1,1};
SDL_Rect pack = {0.0, 0.0};
SDL_Rect camera = {0.0, 100.0};

static void field_set(void);
//キャラの幅を返す関数
int haba(int type){
    switch(type){
    case 0:return ATK_W;
    case 1:return SUP_W;
    case 2:return DEF_W;
    }
}

int zahyo(int type){
    switch(type){
    case 0:return ATK_Y;
    case 1:return SUP_Y;
    case 2:return DEF_Y;
    }
}

//倍率を返す関数
int bai(int type){
    return 1;

}
/* パッドの動きを計算する */
void field_set(void){
    int i,j;
    pad.x += pad.speed_x;
    pad.y += pad.speed_y;
    
    int a = 1;
    if(pad.speed_y < 0){
        /*pad.speed_y = pad.speed_y * (-1);
        pad.y = pad.y * (-1);*/
        a =-1;
    }
    
    //横の壁にぶつかった時
    if(pad.x + PAD_R > FIELD_W){
        pad.x = 2 * FIELD_W - pad.x - 2 * PAD_R;
        pad.speed_x = pad.speed_x * (-1);
    }else if(pad.x - PAD_R < (-1)*FIELD_W){
 	pad.x = (-2) * FIELD_W - pad.x + 2 * PAD_R;
        pad.speed_x = pad.speed_x * (-1);
    }
 /*   //プレイヤーにぶつかった時
    for(j = 0;j < 2;i++){
        if(a == 1)//逆向き
            i = j;
        else
            i = j + 2;
        if(pad.y + PAD_R <= zahyo(p[i].type) && pad.y + PAD_R <= zahyo(p[i].type) + 10 ){
            if(pad.x + PAD_R >= p[i].x && pad.x + PAD_R <= p[i].x + haba(p[i].type) ){
                pad.speed_y = pad.speed_y * (-1) * bai(p[i].type);  //倍率をかけて返す
                p[i].hp -= pad.speed_y;      //ｈｐ減少
                if(p[i].type == 0)           //アタッカーの返す向き
                    pad.speed_x = 0;
                else if(p[i].type == 1){
                    i = i;                   //ｈｐ回復
                }
            }
        }
    }
*/
    //縦の壁にぶつかった時
    if(pad.y + PAD_R > FIELD_H){
        pad.y = 2 * FIELD_H - pad.y - 2*PAD_R;
        pad.speed_y = pad.speed_y * (-1);
    }else if(pad.y - PAD_R <= (-1)*FIELD_H){
 	pad.y = (-2) * FIELD_H - pad.y + 2*PAD_R;
        pad.speed_y = pad.speed_y * (-1);
    }
/*
    if(a == 1){
        pad.speed_y = pad.speed_y * (-1);
        pad.y = pad.y * (-1);
    }
*/
}
int initializeSDL(int flags) {
	// SDLを初期化する
	if (SDL_Init(flags) < 0) {
		fprintf(stderr, "%s\n", SDL_GetError());
		return 1;
	}
	atexit(SDL_Quit);

	return 0;
}

int initializeVideo(int width, int height, int flags) {
	// ビデオモードの設定をする
	if (0 == SDL_SetVideoMode(width, height, 0, flags)) {
		fprintf(stderr, "%s\n", SDL_GetError());
		return 1;
	}

	return 0;
}

int initializeOpenGL(int width, int height) {
	if (initializeVideo(width, height, SDL_OPENGL)) {
		return 1;
	}

	// ビューポートを設定する
	glViewport(0, 0, width, height);
	glClearColor( 1.0, 1.0, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST);

	// 射影行列を設定する
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0, (GLdouble) width / (GLdouble) height, 2.0, 460.0);

	// 照明を設定する
	static GLfloat position[] = {-10.0f, 10.0f, 10.0f, 1.0f};
	static GLfloat ambient [] = { 0.5f, 0.5f, 0.5f, 1.0f};
	static GLfloat diffuse [] = { 0.5f, 0.5f, 0.5f, 1.0f};
	static GLfloat specular[] = { 0.0f, 0.0f, 0.0f, 0.0f};
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	return 0;
}

void draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 視点を設定する
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt( camera.x, camera.y, 90.0f,
	           0.0,  0.0f, 0.0f,
	           0.0f,  0.0f, 1.0f);

	// マテリアルを設定する
	GLfloat position  [] = { 0.0f, 0.0f, 200.0f, 1.0f};
	GLfloat ambient   [] = { 0.1f, 0.1f, 0.1f, 1.0f};
	GLfloat diffuse   [] = { 1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat specular  [] = { 0.0f, 0.0f, 0.0f, 1.0f};
	GLfloat shininess [] = { 0.0f};
	glMaterialfv(GL_FRONT, GL_POSITION, position);
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);



	// 円球を描画する
	GLUquadric* quadric = gluNewQuadric();
	GLUquadric* quadric2= gluNewQuadric();
	glTranslatef(pad.x, pad.y, 0.0f);
	gluCylinder(quadric, 10, 10, 5, 30, 30);
	glTranslatef(0.0, 0.0, 5.0f);
	gluDisk(quadric2, 0, 10, 30, 30);
	glTranslatef(-pad.x, -pad.y, -5.0f);
	gluDeleteQuadric(quadric);
	gluDeleteQuadric(quadric2);

	GLdouble vertex[][3] = {
  	{ 100.0, -160.0, 0.0 },
 	{ 100.1, -160.0, 0.0 },
 	{ 100.1, 160.0, 0.0 },
 	{ 100.0, 160.0, 0.0 },
  	{ 100.0, -160.0, 20.0 },
  	{ 100.1, -160.0, 20.0 },
  	{ 100.1, 160.0, 20.0 },
  	{ 100.0, 160.0, 20.0 }
	};

	GLdouble vertex2[][3] = {
  	{ -100.0, -160.0, 0.0 },
 	{ -100.1, -160.0, 0.0 },
 	{ -100.1, 160.0, 0.0 },
 	{ -100.0, 160.0, 0.0 },
  	{ -100.0, -160.0, 20.0 },
  	{ -100.1, -160.0, 20.0 },
  	{ -100.1, 160.0, 20.0 },
  	{ -100.0, 160.0, 20.0 }
	};

	int edge[][2] = {
	  { 0, 1 },
	  { 1, 2 },
	  { 2, 3 },
	  { 3, 0 },
	  { 4, 5 },
	  { 5, 6 },
	  { 6, 7 },
	  { 7, 4 },
	  { 0, 4 },
	  { 1, 5 },
	  { 2, 6 },
	  { 3, 7 }
	};

	int face[][4] = {
 	{ 0, 1, 2, 3 }, /* A-B-C-D を結ぶ面 */
  	{ 1, 5, 6, 2 }, /* B-F-G-C を結ぶ面 */	
 	{ 5, 4, 7, 6 }, /* F-E-H-G を結ぶ面 */
  	{ 4, 0, 3, 7 }, /* E-A-D-H を結ぶ面 */
  	{ 4, 5, 1, 0 }, /* E-F-B-A を結ぶ面 */
  	{ 3, 2, 6, 7 }  /* D-C-G-H を結ぶ面 */
	};

	int i,j;

  	/* 図形の描画 */
	  glColor3d(0.0, 0.0, 0.0);
	  glBegin(GL_QUADS);
	  for (j = 0; j < 6; ++j) {
    		for (i = 0; i < 4; ++i) {
		      glVertex3dv(vertex[face[j][i]]);
    		}
	  }	  
  	  glEnd();

	  glBegin(GL_QUADS);
	  for (j = 0; j < 6; ++j) {
    		for (i = 0; i < 4; ++i) {
		      glVertex3dv(vertex2[face[j][i]]);
    		}
	  }	  
  	  glEnd();


  	  glFlush();
}
       //大地創造
 void drawAxis(void)
{
    GLdouble axis[][3] =
    {
        { 0.0, 0.0, 0.0 },
        { 1.0, 0.0, 0.0 },
        { 0.0, 1.0, 0.0 },
        { 0.0, 0.0, 1.0 }
    };

    glBegin(GL_LINES);
    int i;
    for( i = 0; i<3; i++)
    {
        glColor3dv(axis[i+1]);
        glVertex3dv(axis[0]);
        glVertex3dv(axis[i+1]);
    }
    glEnd();
}

// XZ平面の描画
void drawPlane(void)
{
    const GLdouble xsize = 100.0f;
    const GLdouble ysize = 160.0f;
    const int xnum = 10;
    const int ynum = 10;
    GLdouble x;
    GLdouble y;

    glBegin(GL_LINES);
    glColor3d(0.5, 0.5, 0.5);
    for( x = -xsize; x <= xsize; x += xsize/xnum)
    {
        glVertex3d( x, -ysize, 0.0 );
        glVertex3d( x, ysize, 0.0 );
    }
    for( y = -ysize; y <= ysize; y += ysize/ynum)
    {
        glVertex3d( -xsize, y, 0.0 );
        glVertex3d(  xsize, y, 0.0 );
    }
    glEnd();
}

int Keyevent(void)
{
	// イベントを処理する
	SDL_Event event;
	if(SDL_PollEvent(&event)) {
		switch (event.type){
			case SDL_QUIT : return 0;
					break;
			case SDL_KEYDOWN : switch(event.key.keysym.sym){
						case SDLK_ESCAPE: return 0;
								  break;
						case SDLK_RIGHT:
								camera.x -= 10;
								break;
						case SDLK_LEFT:
								camera.x += 10;
								break;
						case SDLK_UP:
								camera.y -= 10;
								break;
						case SDLK_DOWN:
								camera.y += 10;
								break;
						case SDLK_d:
								pack.x -= 5;
								break;
						case SDLK_a:
								pack.x += 5;
								break;
						case SDLK_w:
								pack.y -= 5;
								break;
						case SDLK_s:
								pack.y += 5;
								break;
                				default:
								break;
					   }
		}
	}
	return 1;
}

int main(int argc, char** args) {
	if (initializeSDL(SDL_INIT_VIDEO)) {
		return 1;
	}

	if (initializeOpenGL(1024, 768)) {
		return 1;
	}

	int flag = 1;

	while (flag) {
		// イベントを処理する
		field_set();
		flag = Keyevent();
                draw();
                drawPlane();
                drawAxis();
		SDL_GL_SwapBuffers();
	}
}
