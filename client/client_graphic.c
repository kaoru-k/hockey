/*************************************
  client_graphic.c
  クライアントのグラフィック処理
  徳島大学 工学部 知能情報工学科 27班
*************************************/

#include "client.h"
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <GL/glut.h>

PAD pad = {0,0};
SDL_Rect camera = {0.0, 0.0};
int cameramode = 0; //0の時：初期or1p2p / 1の時:3p4p *削除予定

extern int  init_sdl(void);
extern int  draw_field(void);
static int  initializeSDL(int flags);
static int  initializeVideo(int width, int height, int flags);
static int  initializeOpenGL(int width, int height);
static void draw3D(void);
static void draw2D(void);
static void view3D(void);
static void view2D(void);
static void drawAxis(void);
static void drawPlane(void);

int init_sdl(void)
{
    if (initializeSDL(SDL_INIT_VIDEO)) {
        return 1;
    }
    if (initializeOpenGL(1024, 768)) {
        return 1;
    }
}

int draw_field(void)
{
    if(cameramode == 0){
	gluLookAt( camera.x, camera.y, 70.0f,
		   0.0, 0.0f, 0.0f,
	           1.0f,  0.0f, 0.0f);
    }else if(cameramode == 1){
	gluLookAt( camera.x, camera.y, 70.0f,
		   0.0, 0.0f, 0.0f,
	           -1.0f,  0.0f, 0.0f);
    }
    draw3D();
    drawPlane();
    drawAxis();
    draw2D();
    SDL_GL_SwapBuffers();
	
}

static int initializeSDL(int flags) {
    // SDLを初期化する
	if (SDL_Init(flags) < 0) {
		fprintf(stderr, "%s\n", SDL_GetError());
		return 1;
	}
	
	// ジョイスティックサブシステムを初期化する
	SDL_InitSubSystem(SDL_INIT_JOYSTICK);

	joystick = SDL_JoystickOpen(0);	// ジョイスティックを開いて構造体に割り当てる（有効にする）．ここではジョイスティックは1つとして引数に0を指定
	SDL_JoystickEventState(SDL_ENABLE);	// ジョイスティック関連のイベントを取得可能にする

	// ジョイスティックが開けなかったら
	if(!joystick) {
		printf("failed to open joystick.\n");
		exit(-1);
	}
	else{
		printf("The found joystick ID (index) is %d.\n", SDL_JoystickIndex(joystick));	// 接続されたジョイスティックのID番号（1つしか接続されていない場合は0
   		printf("The found joystick has %d axses.\n",SDL_JoystickNumAxes(joystick));	// ジョイスティックの方向キー数を取得
   		printf("The found joystick has %d Hat keys.\n",SDL_JoystickNumHats(joystick));	// ジョイスティックのHatキー数を取得（電算室のジョイスティックでは0?）
   		printf("The found joystick has %d balls.\n",SDL_JoystickNumBalls(joystick));	// ジョイスティックのボール数を取得（電算室のジョイスティックでは0?） */
   		printf("The found joystick has %d buttons.\n",SDL_JoystickNumButtons(joystick));	// ジョイスティックのボタン数を取得
	}

	atexit(SDL_Quit);

	return 0;
}

static int initializeVideo(int width, int height, int flags) {
	// ビデオモードの設定をする
	if (0 == SDL_SetVideoMode(width, height, 0, flags)) {
		fprintf(stderr, "%s\n", SDL_GetError());
		return 1;
	}

	return 0;
}

static int initializeOpenGL(int width, int height) {
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
	gluPerspective(90.0, (GLdouble) width / (GLdouble) height, 1.0, 460.0);

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

//2D描画用
void view2D() {
	glMatrixMode(GL_PROJECTION);// 射影変換行列設定
	//glDisable(GL_LIGHT0);
	//glEnable(GL_LIGHT1);
	glPopMatrix();// 現在の射影変換行列を保存
	//glOrtho(0, 1024, 768, 0, -1, 1);// 正射影変換設定
	glMatrixMode(GL_MODELVIEW);// モデルビュー変換行列設定
	glPopMatrix();// 現在のモデルビュー行列を保存
	glLoadIdentity();// 単位行列を設定
}

//3D描画用
void view3D() {
	
	glMatrixMode(GL_PROJECTION);// 射影変換行列設定
	
	//glEnable(GL_LIGHT0);
	glPushMatrix();// 射影変換行列を復元
	glMatrixMode(GL_MODELVIEW);// モデルビュー変換行列設定
	glPushMatrix();// モデルビュー行列を復元
	glLoadIdentity();// 単位行列を設定
}

void draw2D() {
	view2D();

	GLfloat AP_color[] = { 0.0, 0.0, 1.0, 1.0 };

	if(cameramode == 0){
	GLfloat ATK_HP_color[] = { 1.0, 0.0, 0.0, 1.0 };
  	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ATK_HP_color);
	GLdouble ATK_HP[][2] ={
		{60, 72.5 - (((double)40 / 650) * p[0].hp)},
		{60, 72.5},
		{65, 72.5},
		{65, 72.5 - (((double)40 / 650) * p[0].hp)}

	};

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, AP_color);
	GLdouble ATK_AP[][2] ={
		{50, 32.5},
		{50, 72.5},
		{55, 72.5},
		{55, 32.5}

	};

	GLfloat SUP_HP_color[] = { 0.0, 1.0, 0.0, 1.0 };
  	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, SUP_HP_color);
	GLdouble SUP_HP[][2] ={
		{60, 12.5 - ((42.5 / 300) * p[1].hp)},
		{60, 12.5},
		{65, 12.5},
		{65, 12.5 - ((42.5 / 300) * p[1].hp)}

	};

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, AP_color);
	GLdouble SUP_AP[][2] ={
		{50, -30},
		{50, 12.5},
		{55, 12.5},
		{55, -30}

	};

	GLfloat DEF_HP_color[] = { 0.0, 0.0, 1.0, 1.0 };
  	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, DEF_HP_color);
	GLdouble DEF_HP[][2] ={
		{60, -50 - (((double)40 / 800) * p[4].hp)},
		{60, -50},
		{65, -50},
		{65, -50 - (((double)40 / 800) * p[4].hp)}

	};

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, AP_color);
	GLdouble DEF_AP[][2] ={
		{50, -50},
		{50, -90},
		{55, -90},
		{55, -50}

	};

	int i;
	glBegin(GL_QUADS);
    	for (i = 0; i < 4; ++i) {
		      glVertex2dv(ATK_HP[i]);
	  }	  
  	glEnd();

	glBegin(GL_QUADS);
    	for (i = 0; i < 4; ++i) {
		      glVertex2dv(ATK_AP[i]);
	  }	  
  	glEnd();

	glBegin(GL_QUADS);
    	for (i = 0; i < 4; ++i) {
		      glVertex2dv(SUP_HP[i]);
	  }	  
  	glEnd();

	glBegin(GL_QUADS);
    	for (i = 0; i < 4; ++i) {
		      glVertex2dv(SUP_AP[i]);
	  }	  
  	glEnd();

	glBegin(GL_QUADS);
    	for (i = 0; i < 4; ++i) {
		      glVertex2dv(DEF_HP[i]);
	  }	  
  	glEnd();

	glBegin(GL_QUADS);
    	for (i = 0; i < 4; ++i) {
		      glVertex2dv(DEF_AP[i]);
	  }	  
  	glEnd();
	}else if(cameramode == 1){
	GLfloat ATK_HP_color[] = { 1.0, 0.0, 0.0, 1.0 };
  	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ATK_HP_color);
	GLdouble ATK_HP2[][2] ={
		{60, 72.5 - (((double)45 / 550) * p[2].hp)},
		{60, 72.5},
		{65, 72.5},
		{65, 72.5 - (((double)45 / 550) * p[2].hp)}

	};

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, AP_color);
	GLdouble ATK_AP2[][2] ={
		{50, 32.5},
		{50, 72.5},
		{55, 72.5},
		{55, 32.5}

	};

	GLfloat SUP_HP_color[] = { 0.0, 1.0, 0.0, 1.0 };
  	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, SUP_HP_color);
	GLdouble SUP_HP2[][2] ={
		{60, 12.5 - (((double)42.5 / 450) * p[3].hp)},
		{60, 12.5},
		{65, 12.5},
		{65, 12.5 - (((double)42.5 / 450) * p[3].hp)}

	};

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, AP_color);
	GLdouble SUP_AP2[][2] ={
		{50, -30},
		{50, 12.5},
		{55, 12.5},
		{55, -30}

	};

	GLfloat DEF_HP_color[] = { 0.0, 0.0, 1.0, 1.0 };
  	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, DEF_HP_color);
	GLdouble DEF_HP2[][2] ={
		{60, -50 - (((double)40 / 800) * p[5].hp)},
		{60, -50},
		{65, -50},
		{65, -50 - (((double)40 / 800) * p[5].hp)}

	};

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, AP_color);
	GLdouble DEF_AP2[][2] ={
		{50, -50},
		{50, -90},
		{55, -90},
		{55, -50}

	};

	int i;
	glBegin(GL_QUADS);
    	for (i = 0; i < 4; ++i) {
		      glVertex2dv(ATK_HP2[i]);
	  }	  
  	glEnd();

	glBegin(GL_QUADS);
    	for (i = 0; i < 4; ++i) {
		      glVertex2dv(ATK_AP2[i]);
	  }	  
  	glEnd();

	glBegin(GL_QUADS);
    	for (i = 0; i < 4; ++i) {
		      glVertex2dv(SUP_HP2[i]);
	  }	  
  	glEnd();

	glBegin(GL_QUADS);
    	for (i = 0; i < 4; ++i) {
		      glVertex2dv(SUP_AP2[i]);
	  }	  
  	glEnd();

	glBegin(GL_QUADS);
    	for (i = 0; i < 4; ++i) {
		      glVertex2dv(DEF_HP2[i]);
	  }	  
  	glEnd();

	glBegin(GL_QUADS);
    	for (i = 0; i < 4; ++i) {
		      glVertex2dv(DEF_AP2[i]);
	  }	  
  	glEnd();
	}	
}


static void draw3D(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    view3D;

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
    

    GLfloat white[] = { 0.654, 0.243, 1.0, 1.0 };
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, white);
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
    
    GLdouble sidevertex[][3] = {
      { 100.0, -160.0, 0.0 },
      { 100.1, -160.0, 0.0 },
      { 100.1, 160.0, 0.0 },
      { 100.0, 160.0, 0.0 },
      { 100.0, -160.0, 20.0 },
      { 100.1, -160.0, 20.0 },
      { 100.1, 160.0, 20.0 },
      { 100.0, 160.0, 20.0 }
    };
    
    GLdouble sidevertex2[][3] = {
      { -100.0, -160.0, 0.0 },
      { -100.1, -160.0, 0.0 },
      { -100.1, 160.0, 0.0 },
      { -100.0, 160.0, 0.0 },
      { -100.0, -160.0, 20.0 },
      { -100.1, -160.0, 20.0 },
      { -100.1, 160.0, 20.0 },
      { -100.0, 160.0, 20.0 }
    };

    GLdouble Goalpost[][3] = {
      { GOAL_W, 160.0, 0.0 },
      { 100.0, 160.0, 0.0 },
      { 100.0, 160.1, 0.0 },
      { GOAL_W, 160.1, 0.0 },
      { GOAL_W, 160.0, 10.0 },
      { 100.0, 160.0, 10.0 },
      { 100.0, 160.1, 10.0 },
      { GOAL_W, 160.1, 10.0 }
    };

    GLdouble Goalpost2[][3] = {
      { -GOAL_W, 160.0, 0.0 },
      { -100.0, 160.0, 0.0 },
      { -100.0, 160.1, 0.0 },
      { -GOAL_W, 160.1, 0.0 },
      { -GOAL_W, 160.0, 10.0 },
      { -100.0, 160.0, 10.0 },
      { -100.0, 160.1, 10.0 },
      { -GOAL_W, 160.1, 10.0 }
    };

    GLdouble Goalpost3[][3] = {
      { GOAL_W, -160.0, 0.0 },
      { 100.0, -160.0, 0.0 },
      { 100.0, -160.1, 0.0 },
      { GOAL_W, -160.1, 0.0 },
      { GOAL_W, -160.0, 10.0 },
      { 100.0, -160.0, 10.0 },
      { 100.0, -160.1, 10.0 },
      { GOAL_W, -160.1, 10.0 }
    };

    GLdouble Goalpost4[][3] = {
      { -GOAL_W, -160.0, 0.0 },
      { -100.0, -160.0, 0.0 },
      { -100.0, -160.1, 0.0 },
      { -GOAL_W, -160.1, 0.0 },
      { -GOAL_W, -160.0, 10.0 },
      { -100.0, -160.0, 10.0 },
      { -100.0, -160.1, 10.0 },
      { -GOAL_W, -160.1, 10.0 }
    };
    
    GLdouble atk1_vertex[][3] = {
      { p[0].x - ATK_W, ATK_Y, 0.0 },
      { p[0].x + ATK_W, ATK_Y, 0.0 },
      { p[0].x + ATK_W, ATK_Y + 5, 0.0 },
      { p[0].x - ATK_W, ATK_Y + 5, 0.0 },
      { p[0].x - ATK_W, ATK_Y, 5.0 },
      { p[0].x + ATK_W, ATK_Y, 5.0 },
      { p[0].x + ATK_W, ATK_Y + 5, 5.0 },
      { p[0].x - ATK_W, ATK_Y + 5, 5.0 }
    };

	GLdouble sup1_vertex[][3] = {
  	{ p[1].x - SUP_W, SUP_Y, 0.0 },
 	{ p[1].x + SUP_W, SUP_Y, 0.0 },
 	{ p[1].x + SUP_W, SUP_Y + 5, 0.0 },
 	{ p[1].x - SUP_W, SUP_Y + 5, 0.0 },
  	{ p[1].x - SUP_W, SUP_Y, 5.0 },
  	{ p[1].x + SUP_W, SUP_Y, 5.0 },
  	{ p[1].x + SUP_W, SUP_Y + 5, 5.0 },
  	{ p[1].x - SUP_W, SUP_Y + 5, 5.0 }
	};

	GLdouble def1_vertex[][3] = {
  	{ p[4].x - DEF_W, DEF_Y, 0.0 },
 	{ p[4].x + DEF_W, DEF_Y, 0.0 },
 	{ p[4].x + DEF_W, DEF_Y + 5, 0.0 },
 	{ p[4].x - DEF_W, DEF_Y + 5, 0.0 },
  	{ p[4].x - DEF_W, DEF_Y, 5.0 },
  	{ p[4].x + DEF_W, DEF_Y, 5.0 },
  	{ p[4].x + DEF_W, DEF_Y + 5, 5.0 },
  	{ p[4].x - DEF_W, DEF_Y + 5, 5.0 }
	};

	GLdouble atk2_vertex[][3] = {
  	{ p[2].x - ATK_W, -ATK_Y, 0.0 },
 	{ p[2].x + ATK_W, -ATK_Y, 0.0 },
 	{ p[2].x + ATK_W, -ATK_Y - 5, 0.0 },
 	{ p[2].x - ATK_W, -ATK_Y - 5, 0.0 },
  	{ p[2].x - ATK_W, -ATK_Y, 5.0 },
  	{ p[2].x + ATK_W, -ATK_Y, 5.0 },
  	{ p[2].x + ATK_W, -ATK_Y - 5, 5.0 },
  	{ p[2].x - ATK_W, -ATK_Y - 5, 5.0 }
	};

	GLdouble sup2_vertex[][3] = {
  	{ p[3].x - SUP_W, -SUP_Y, 0.0 },
 	{ p[3].x + SUP_W, -SUP_Y, 0.0 },
 	{ p[3].x + SUP_W, -SUP_Y - 5, 0.0 },
 	{ p[3].x - SUP_W, -SUP_Y - 5, 0.0 },
  	{ p[3].x - SUP_W, -SUP_Y, 5.0 },
  	{ p[3].x + SUP_W, -SUP_Y, 5.0 },
  	{ p[3].x + SUP_W, -SUP_Y - 5, 5.0 },
  	{ p[3].x - SUP_W, -SUP_Y - 5, 5.0 }
	};

	GLdouble def2_vertex[][3] = {
  	{ p[5].x - DEF_W, -DEF_Y, 0.0 },
 	{ p[5].x + DEF_W, -DEF_Y, 0.0 },
 	{ p[5].x + DEF_W, -DEF_Y - 5, 0.0 },
 	{ p[5].x - DEF_W, -DEF_Y - 5, 0.0 },
  	{ p[5].x - DEF_W, -DEF_Y, 5.0 },
  	{ p[5].x + DEF_W, -DEF_Y, 5.0 },
  	{ p[5].x + DEF_W, -DEF_Y - 5, 5.0 },
  	{ p[5].x - DEF_W, -DEF_Y - 5, 5.0 }
	};

	GLdouble score[][3] = {
		{90, 133.1, 98},//lu
		{90, 122, 125.2},//ld
		{-90, 122, 125.2},//rd
		{-90, 133.1, 98}//ru
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

	GLfloat grey[] = { 0.0, 1.0, 0.69, 1.0 };
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, grey);
  	/* 図形の描画 */
	  glColor3d(0.0, 0.0, 0.0);
	  glBegin(GL_QUADS);
	  for (j = 0; j < 6; ++j) {
    		for (i = 0; i < 4; ++i) {
		      glVertex3dv(sidevertex[face[j][i]]);
    		}
	  }	  
  	  glEnd();

	  glBegin(GL_QUADS);
	  for (j = 0; j < 6; ++j) {
    		for (i = 0; i < 4; ++i) {
		      glVertex3dv(sidevertex2[face[j][i]]);
    		}
	  }	  
  	  glEnd();

          glBegin(GL_QUADS);
	  for (j = 0; j < 6; ++j) {
    		for (i = 0; i < 4; ++i) {
		      glVertex3dv(Goalpost[face[j][i]]);
    		}
	  }	  
  	  glEnd();

	  glBegin(GL_QUADS);
	  for (j = 0; j < 6; ++j) {
    		for (i = 0; i < 4; ++i) {
		      glVertex3dv(Goalpost2[face[j][i]]);
    		}
	  }	  
  	  glEnd();

	glBegin(GL_QUADS);
	  for (j = 0; j < 6; ++j) {
    		for (i = 0; i < 4; ++i) {
		      glVertex3dv(Goalpost3[face[j][i]]);
    		}
	  }	  
  	  glEnd();

	  glBegin(GL_QUADS);
	  for (j = 0; j < 6; ++j) {
    		for (i = 0; i < 4; ++i) {
		      glVertex3dv(Goalpost4[face[j][i]]);
    		}
	  }	  
  	  glEnd();

	GLfloat g[] = { 0.0, 1.0, 0.32, 1.0 };
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, g);
	  glBegin(GL_QUADS);
	  for (j = 0; j < 6; ++j) {
    		for (i = 0; i < 4; ++i) {
		      glVertex3dv(atk1_vertex[face[j][i]]);
    		}
	  }	  
  	  glEnd();

	glBegin(GL_QUADS);
	  for (j = 0; j < 6; ++j) {
    		for (i = 0; i < 4; ++i) {
		      glVertex3dv(sup1_vertex[face[j][i]]);
    		}
	  }	  
  	  glEnd();

	glBegin(GL_QUADS);
	  for (j = 0; j < 6; ++j) {
    		for (i = 0; i < 4; ++i) {
		      glVertex3dv(def1_vertex[face[j][i]]);
    		}
	  }	  
  	  glEnd();

	  glBegin(GL_QUADS);
	  for (j = 0; j < 6; ++j) {
    		for (i = 0; i < 4; ++i) {
		      glVertex3dv(atk2_vertex[face[j][i]]);
    		}
	  }	  
  	  glEnd();

	  glBegin(GL_QUADS);
	  for (j = 0; j < 6; ++j) {
    		for (i = 0; i < 4; ++i) {
		      glVertex3dv(sup2_vertex[face[j][i]]);
    		}
	  }	  
  	  glEnd();

	  glBegin(GL_QUADS);
	  for (j = 0; j < 6; ++j) {
    		for (i = 0; i < 4; ++i) {
		      glVertex3dv(def2_vertex[face[j][i]]);
    		}
	  }	  
  	  glEnd();

	  /*glBegin(GL_QUADS);
		for (i = 0; i < 4; ++i) {
		      glVertex3dv(score[face[0][i]]);
    		}
	  glEnd();*/

  	  glFlush();
}

//大地創造
static void drawAxis(void)
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
static void drawPlane(void)
{
    const GLdouble xsize = 100.0f;
    const GLdouble ysize = 160.0f;
    const int xnum = 10;
    const int ynum = 10;
    GLdouble x;
    GLdouble y;


    GLfloat black[] = { 0.0, 0.0, 0.0, 1.0 };
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, black);
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

/*必殺技（致命傷）*/
int onoff(void){
	int i = 0;
	static GLfloat positionh[4];
	static GLfloat ambient [] = { 0.5f, 0.5f, 0.5f, 1.0f};
	static GLfloat diffuse [] = { 0.5f, 0.5f, 0.5f, 1.0f};
	static GLfloat specular[] = { 0.0f, 0.0f, 0.0f, 0.0f};
	GLfloat yellow[] = { 1.0, 1.0, 0.0, 1.0 };

		
	positionh[0] = pad.x;
	positionh[1] = pad.y;
	positionh[2] = 0.0f;
	positionh[3] = 1.0f;
	
	i++;
	if(i == 5){
	glLightfv(GL_LIGHT1, GL_POSITION, positionh);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, yellow);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
	glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1);
	
	printf("a\n");
	}
	
	if(i == 10){
	glLightfv(GL_LIGHT1, GL_POSITION, positionh);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, yellow);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
	glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	glEnable(GL_LIGHTING);
	glDisable(GL_LIGHT1);
 	i = 0;
	printf("b\n");
	}
 	
	

	return 0;
}


