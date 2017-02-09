/*************************************
  client_graphic.c
  クライアントのグラフィック処理
  徳島大学 工学部 知能情報工学科 27班
*************************************/

#include "client.h"
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <GL/glut.h>


#define REC      32
#define WINDOW_W 1024
#define WINDOW_H 768

PAD         pad          = {0,0};
int         cameramode   = 0;             // 0の時：初期or1p2p / 1の時:3p4p
SDL_Rect    camera       = {0.0, 0.0};
GLuint      texA[6]      = {0};           // キャラテクスチャ
GLuint      Starttex     = 0;		//スタート画面
GLuint	    AP	         = 0;		//必殺技ゲージ（満タン時用）
GLuint      Result[2]    = {0};           //結果画像
GLuint      GOAL         = 0;
GLuint      PointTex[10] = {0};
GLuint      mozi[6]      = {0};
GLuint      cont_img     = 0;
GLuint      hassha_img   = 0;
GLuint      Button[2]    = {0};
int         flash        = 0;
GLUquadric* quadric;
GLUquadric* quadric2;
GLUquadric* quadric3;

static void set_OpenGL(void);
static void draw3D(void);
static void draw2D(void);
static void view3D(void);
static void view2D(void);
static void drawAxis(void);
static void drawPlane(void);
static int  Pot(int inSize);
static void creatTex(char *file, GLuint *tex);
static void modelD(GLdouble alp);
static void modelD_test(void);
static void flash_on(void);
static void flash_off(void);

int init_sdl(void)
{
    char title[50];
    SDL_Surface* icon = SDL_LoadBMP("image/KBTIT.bmp");
    // SDLを初期化する
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_SetVideoMode(WINDOW_W, WINDOW_H, 0, SDL_OPENGL);
    sprintf(title, "AIR HOCKEY [myid=%d]", myid);
    SDL_WM_SetCaption(title, "AIR HOCKEY");
    SDL_WM_SetIcon(icon, NULL);
    set_OpenGL();

    // ジョイスティックサブシステムを初期化する
    SDL_InitSubSystem(SDL_INIT_JOYSTICK);
    joystick = SDL_JoystickOpen(0);	// ジョイスティックを開いて構造体に割り当てる（有効にする）．ここではジョイスティックは1つとして引数に0を指定
    SDL_JoystickEventState(SDL_ENABLE);	// ジョイスティック関連のイベントを取得可能にする

    // ジョイスティックが開けなかったら
    if(!joystick)
        printf("failed to open joystick.\n");

    //テクスチャ作成
    creatTex("./image/mcree.bmp", &texA[0]);
    creatTex("./image/reaper.bmp", &texA[1]);
    creatTex("./image/macy.bmp", &texA[2]);
    creatTex("./image/zeni.bmp", &texA[3]);
    creatTex("./image/Dva.bmp", &texA[4]); 
    creatTex("./image/zaria.bmp", &texA[5]);
    creatTex("./image/H.bmp", &Starttex);
    creatTex("./image/hs.bmp", &AP);
    creatTex("./image/GOAL.bmp", &GOAL);
    creatTex("./image/kakuteiv.bmp", &Result[0]);
    creatTex("./image/kakuteid.bmp", &Result[1]);
    creatTex("./image/0.bmp", &PointTex[0]);
    creatTex("./image/1.bmp", &PointTex[1]);
    creatTex("./image/2.bmp", &PointTex[2]);
    creatTex("./image/3.bmp", &PointTex[3]);
    creatTex("./image/4.bmp", &PointTex[4]);
    creatTex("./image/5.bmp", &PointTex[5]);
    creatTex("./image/6.bmp", &PointTex[6]);
    creatTex("./image/7.bmp", &PointTex[7]);
    creatTex("./image/8.bmp", &PointTex[8]);
    creatTex("./image/9.bmp", &PointTex[9]);
    creatTex("./image/10.bmp",&PointTex[10]);
    creatTex("./image/ten.bmp",&mozi[0]);
    creatTex("./image/sen.bmp",&mozi[1]);
    creatTex("./image/syu.bmp",&mozi[2]);
    creatTex("./image/de.bmp",&mozi[3]);
    creatTex("./image/syou.bmp",&mozi[4]);
    creatTex("./image/ri.bmp",&mozi[5]);
    creatTex("./image/hassha.bmp", &hassha_img);
    creatTex("./image/continue.bmp",&cont_img);
    creatTex("./image/button_2.bmp", &Button[0]);
    creatTex("./image/button_1.bmp", &Button[1]);


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
    modelD_test();
    SDL_GL_SwapBuffers();
}

static void set_OpenGL(void)
{
    // ビューポートを設定する
    glViewport(0, 0, WINDOW_W, WINDOW_H);
    glClearColor( 1.0, 1.0, 1.0, 1.0);
    glEnable(GL_DEPTH_TEST);

    // 射影行列を設定する
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90.0, (GLdouble) WINDOW_W / (GLdouble) WINDOW_H, 1.0, 460.0);
    
    // 照明を設定する
    static GLfloat position[] = {-10.0f, 10.0f, 10.0f, 1.0f};
    static GLfloat ambient [] = { 0.9f, 0.9f, 0.9f, 1.0f};
    static GLfloat diffuse [] = { 0.0f, 0.0f, 0.0f, 0.0f};
    static GLfloat specular[] = { 0.0f, 0.0f, 0.0f, 0.0f};

    static GLfloat ambient1 [] = { 1.0f, 1.0f, 1.0f, 1.0f};
    static GLfloat diffuse1[] = { 1.0, 1.0, 0.0, 1.0 };
    static GLfloat specular1[] = { 0.0f, 0.0f, 0.0f, 0.0f};

    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse1);
    glLightfv(GL_LIGHT1, GL_SPECULAR, specular1);
    glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    quadric = gluNewQuadric();
    quadric2 = gluNewQuadric();
    quadric3 = gluNewQuadric();



}

void StartWindow(void)
{
    gluLookAt( 0.0f, 0.0f, 70.0f,
	       0.0, 0.0f, 0.0f,
	       1.0f,  0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    view3D();
    view2D();

    glLoadIdentity();
    glEnable(GL_TEXTURE_2D);//テクスチャON

    static GLdouble alp,vec;
    if     ( (vec==0)&&(alp+=0.01)>1){alp=1; vec=1;}//透過計算
    else if( (vec==1)&&(alp-=0.01)<0){alp=0; vec=0;}
    

    glBindTexture(GL_TEXTURE_2D, Starttex);
    GLfloat texture_color[] = {1, 1, 1, alp};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, texture_color);

    glBegin(GL_QUADS);
    glTexCoord2i(0, 0);  glVertex2i( 70, 94);
    glTexCoord2i(1, 0);  glVertex2i( 70,-94);
    glTexCoord2i(1, 1);  glVertex2i(-70,-94);
    glTexCoord2i(0, 1);  glVertex2i(-70, 94);
    glEnd();

    glDisable(GL_TEXTURE_2D);//テクスチャOFF

    SDL_GL_SwapBuffers();
}

void SettingWindow(void)
{
    view3D();
    view2D();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    glEnable(GL_TEXTURE_2D);//テクスチャON

    static GLdouble alp,vec;
    if     ( (vec==0)&&(alp+=0.01)>1){alp=1; vec=1;}//透過計算
    else if( (vec==1)&&(alp-=0.01)<0){alp=0; vec=0;}

    int i[4] = {0};
    i[setting.chara] = 1;

    GLfloat texture_color[] = {1, 1, 1, alp};
    GLfloat texture_color2[] = {0, 0, 1, alp};
    glBindTexture(GL_TEXTURE_2D, texA[0]);
    
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, texture_color);
    if(settingflag != 0 && i[0] == 0)
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, texture_color2);
    glBegin(GL_QUADS);
    glTexCoord2i(0, 0);  glVertex2i( 45 + (5 * i[0]), 75 + (5 * i[0]));
    glTexCoord2i(1, 0);  glVertex2i( 45 + (5 * i[0]), 55 - (5 * i[0]));
    glTexCoord2i(1, 1);  glVertex2i( 25 - (5 * i[0]), 55 - (5 * i[0]));
    glTexCoord2i(0, 1);  glVertex2i( 25 - (5 * i[0]), 75 + (5 * i[0]));
    glEnd();

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, texture_color);
    if(settingflag != 0 && i[1] == 0)
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, texture_color2);
    glBindTexture(GL_TEXTURE_2D, texA[1]);
    glBegin(GL_QUADS);
    glTexCoord2i(0, 0);  glVertex2i( 45 + (5 * i[1]), 30 + (5 * i[1]));
    glTexCoord2i(1, 0);  glVertex2i( 45 + (5 * i[1]), 10 - (5 * i[1]));
    glTexCoord2i(1, 1);  glVertex2i( 25 - (5 * i[1]), 10 - (5 * i[1]));
    glTexCoord2i(0, 1);  glVertex2i( 25 - (5 * i[1]), 30 + (5 * i[1]));
    glEnd();

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, texture_color);
    if(settingflag != 0 && i[2] == 0)
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, texture_color2);
    glBindTexture(GL_TEXTURE_2D, texA[2]);
    glBegin(GL_QUADS);
    glTexCoord2i(0, 0);  glVertex2i( 45 + (5 * i[2]), -10 + (5 * i[2]));
    glTexCoord2i(1, 0);  glVertex2i( 45 + (5 * i[2]), -30 - (5 * i[2]));
    glTexCoord2i(1, 1);  glVertex2i( 25 - (5 * i[2]), -30 - (5 * i[2]));
    glTexCoord2i(0, 1);  glVertex2i( 25 - (5 * i[2]), -10 + (5 * i[2]));
    glEnd();

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, texture_color);
    if(settingflag != 0 && i[3] == 0)
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, texture_color2);
    glBindTexture(GL_TEXTURE_2D, texA[3]);
    glBegin(GL_QUADS);
    glTexCoord2i(0, 0);  glVertex2i( 45 + (5 * i[3]), -55 + (5 * i[3]));
    glTexCoord2i(1, 0);  glVertex2i( 45 + (5 * i[3]), -75 - (5 * i[3]));
    glTexCoord2i(1, 1);  glVertex2i( 25 - (5 * i[3]), -75 - (5 * i[3]));
    glTexCoord2i(0, 1);  glVertex2i( 25 - (5 * i[3]), -55 + (5 * i[3]));
    glEnd();

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, texture_color);
    if(myid == 0){
    glBindTexture(GL_TEXTURE_2D, PointTex[setting.point]);
	if(settingflag == 0 || settingflag == 2){
    		glBegin(GL_QUADS);
    		glTexCoord2i(0, 0);  glVertex2i( -10, 10 );
    		glTexCoord2i(1, 0);  glVertex2i( -10,-10 );
    		glTexCoord2i(1, 1);  glVertex2i( -30,-10 );
    		glTexCoord2i(0, 1);  glVertex2i( -30, 10 );
                glEnd();
	}
	if(settingflag == 1){
    		glBegin(GL_QUADS);
    		glTexCoord2i(0, 0);  glVertex2i( -5, 15 );
    		glTexCoord2i(1, 0);  glVertex2i( -5,-15 );
    		glTexCoord2i(1, 1);  glVertex2i( -35,-15 );
    		glTexCoord2i(0, 1);  glVertex2i( -35, 15 );
                glEnd();
	}
    }
    
    if((myid == 0 && settingflag == 2 )|| (myid != 0 && settingflag == 1 )){
    glBindTexture(GL_TEXTURE_2D, Button[1]);
    glBegin(GL_QUADS);
    glTexCoord2i(0, 0);  glVertex2i( -40 , 20);
    glTexCoord2i(1, 0);  glVertex2i( -40 ,-20);
    glTexCoord2i(1, 1);  glVertex2i( -60 ,-20);
    glTexCoord2i(0, 1);  glVertex2i( -60 , 20);
    glEnd();
    }else{
    glBindTexture(GL_TEXTURE_2D, Button[0]);
    glBegin(GL_QUADS);
    glTexCoord2i(0, 0);  glVertex2i( -40 , 20);
    glTexCoord2i(1, 0);  glVertex2i( -40 ,-20);
    glTexCoord2i(1, 1);  glVertex2i( -60 ,-20);
    glTexCoord2i(0, 1);  glVertex2i( -60 , 20);
    glEnd();
    }

    glBindTexture(GL_TEXTURE_2D, Starttex);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, texture_color2);

    glBegin(GL_QUADS);
    glTexCoord2i(0, 0);  glVertex2i( 70, 94);
    glTexCoord2i(1, 0);  glVertex2i( 70,-94);
    glTexCoord2i(1, 1);  glVertex2i(-70,-94);
    glTexCoord2i(0, 1);  glVertex2i(-70, 94);
    glEnd();

    glDisable(GL_TEXTURE_2D);//テクスチャOFF

    SDL_GL_SwapBuffers();
}

void WaitingWindow(void)
{
    view3D();
    view2D();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    glEnable(GL_TEXTURE_2D);//テクスチャON
    static GLdouble alp,vec;
    if     ( (vec==0)&&(alp+=0.01)>1){alp=1; vec=1;}//透過計算
    else if( (vec==1)&&(alp-=0.01)<0){alp=0; vec=0;}

    GLfloat texture_color[] = {1, 1, 1, alp};
    GLfloat texture_color2[] = {0, 0, 1, alp};

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, texture_color);
    glBindTexture(GL_TEXTURE_2D, texA[p[control_id].type]);
    glBegin(GL_QUADS);
    glTexCoord2i(0, 0);  glVertex2i( 50 , 10 );
    glTexCoord2i(1, 0);  glVertex2i( 50 ,-10 );
    glTexCoord2i(1, 1);  glVertex2i( 30 ,-10 );
    glTexCoord2i(0, 1);  glVertex2i( 30 , 10 );
    glEnd();

    glBindTexture(GL_TEXTURE_2D, PointTex[setting2.point]);
    glBegin(GL_QUADS);
    glTexCoord2i(0, 0);  glVertex2i(-20 , 60 );
    glTexCoord2i(1, 0);  glVertex2i(-20 , 40 );
    glTexCoord2i(1, 1);  glVertex2i(-40 , 40 );
    glTexCoord2i(0, 1);  glVertex2i(-40 , 60 );
    glEnd();

    int i;
    for(i = 0; i < 6; i++){
	glBindTexture(GL_TEXTURE_2D, mozi[i]);
        glBegin(GL_QUADS);
        glTexCoord2i(0, 0);  glVertex2i(-25 , 40 -(i * 10));
        glTexCoord2i(1, 0);  glVertex2i(-25 , 30 -(i * 10));
        glTexCoord2i(1, 1);  glVertex2i(-35 , 30 -(i * 10));
        glTexCoord2i(0, 1);  glVertex2i(-35 , 40 -(i * 10));
        glEnd();
    }

    glBindTexture(GL_TEXTURE_2D, Starttex);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, texture_color2);

    glBegin(GL_QUADS);
    glTexCoord2i(0, 0);  glVertex2i( 70, 94);
    glTexCoord2i(1, 0);  glVertex2i( 70,-94);
    glTexCoord2i(1, 1);  glVertex2i(-70,-94);
    glTexCoord2i(0, 1);  glVertex2i(-70, 94);
    glEnd();

    glDisable(GL_TEXTURE_2D);//テクスチャOFF
    SDL_GL_SwapBuffers();
}

//2D描画用
static void view2D()
{
    glMatrixMode(GL_PROJECTION);         // 射影変換行列設定
    //glDisable(GL_LIGHT0);
    //glEnable(GL_LIGHT1);
    glPopMatrix();                       // 現在の射影変換行列を保存
    //glOrtho(0, 1024, 768, 0, -1, 1);   // 正射影変換設定
    glMatrixMode(GL_MODELVIEW);          // モデルビュー変換行列設定
    glPopMatrix();                       // 現在のモデルビュー行列を保存
    glLoadIdentity();                    // 単位行列を設定
}

//3D描画用
static void view3D()
{
    glMatrixMode(GL_PROJECTION);// 射影変換行列設定	
    //glEnable(GL_LIGHT0);
    glPushMatrix();// 射影変換行列を復元
    glMatrixMode(GL_MODELVIEW);// モデルビュー変換行列設定
    glPushMatrix();// モデルビュー行列を復元
    glLoadIdentity();// 単位行列を設定
}

//引数：キャラのタイプ　　そのタイプのHPを返す
int mHP(int type){
    switch(type){
	case 0:return ATK1_HP0;
	case 1:return ATK2_HP0;
	case 2:return SUP1_HP0;
	case 3:return SUP2_HP0;
	default:return 999;
    }
}


void draw2D()
{	
    view2D();

    GLfloat AP_color[] = { 0.0, 1.0, 1.0, 1.0 };
    GLfloat AP_MAX_color[] = { 1.0, 1.0, 0.0, 1.0 };
    GLfloat ATK_HP_color[] = { 1.0, 0.0, 0.0, 1.0 };
    GLfloat SUP_HP_color[] = { 0.0, 1.0, 0.0, 1.0 };
    GLfloat DEF_HP_color[] = { 0.0, 0.0, 1.0, 1.0 };

    if(cameramode == 0){
	GLdouble ATK_HP[][2] ={
            {60, 72.5 - (((double)40 / mHP(p[0].type)) * p[0].hp)},
            {60, 72.5},
            {65, 72.5},
            {65, 72.5 - (((double)40 / mHP(p[0].type)) * p[0].hp)}   
	};

	GLdouble ATK_AP[][2] ={
            {50, 72.5 - (((double)40 / 100) * p[0].ap)},
            {50, 72.5},
            {55, 72.5},
            {55, 72.5 - (((double)40 / 100) * p[0].ap)}
	};

	GLdouble SUP_HP[][2] ={
            {60, 12.5 - (((double)42.5 / mHP(p[1].type)) * p[1].hp)},
            {60, 12.5},
            {65, 12.5},
            {65, 12.5 - (((double)42.5 / mHP(p[1].type)) * p[1].hp)}
	};

	GLdouble SUP_AP[][2] ={
            {50, 12.5 - (((double)42.5 / 100) * p[1].ap)},
            {50, 12.5},
            {55, 12.5},
            {55, 12.5 - (((double)42.5 / 100) * p[1].ap)}
	};

	GLdouble DEF_HP[][2] ={
            {60, -50 - (((double)40 / 800) * p[4].hp)},
            {60, -50},
            {65, -50},
            {65, -50 - (((double)40 / 800) * p[4].hp)}
	};

	int i;

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ATK_HP_color);
	glBegin(GL_QUADS);
    	for (i = 0; i < 4; ++i) {
            glVertex2dv(ATK_HP[i]);
        }	  
  	glEnd();

/*	if(p[0].ap == 100){
        glEnable(GL_TEXTURE_2D);//テクスチャON
        glBindTexture(GL_TEXTURE_2D, AP);
        GLfloat texture_color[] = {1, 1, 1, 1};
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, texture_color);
    
	for(i = 0; i <  8; i++){
        glBegin(GL_QUADS);
        glTexCoord2i(0, 0);  glVertex2d( 55, 72.5 - (5 * i) );
        glTexCoord2i(1, 0);  glVertex2d( 55, 72.5 - (5 * (i + 1)) );
        glTexCoord2i(1, 1);  glVertex2d( 50, 72.5 - (5 * (i + 1)) );
        glTexCoord2i(0, 1);  glVertex2d( 50, 72.5 - (5 * i) );
        glEnd();
        }
	glDisable(GL_TEXTURE_2D);//テクスチャOFF
        }else{
*/
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, AP_color);

        if(p[0].ap == 100)
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, AP_MAX_color);

	glBegin(GL_QUADS);
    	for (i = 0; i < 4; ++i) {
	    glVertex2dv(ATK_AP[i]);
        }	  
  	glEnd();

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, SUP_HP_color);
	glBegin(GL_QUADS);
    	for (i = 0; i < 4; ++i) {
            glVertex2dv(SUP_HP[i]);
        }	  
  	glEnd();

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, AP_color);
        if(p[1].ap == 100)
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, AP_MAX_color);

	glBegin(GL_QUADS);
    	for (i = 0; i < 4; ++i) {
            glVertex2dv(SUP_AP[i]);
        }	  
  	glEnd();

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, DEF_HP_color);
	glBegin(GL_QUADS);
    	for (i = 0; i < 4; ++i) {
            glVertex2dv(DEF_HP[i]);
        }	  
  	glEnd();

/*	glBegin(GL_QUADS);
    	for (i = 0; i < 4; ++i) {
		      glVertex2dv(DEF_AP[i]);
        }	  
  	glEnd();*/
    }
    else if(cameramode == 1){
	GLdouble ATK_HP2[][2] ={
            {60, 72.5 - (((double)40 / mHP(p[2].type)) * p[2].hp)},
            {60, 72.5},
            {65, 72.5},
            {65, 72.5 - (((double)40 / mHP(p[2].type)) * p[2].hp)}
	};

	GLdouble ATK_AP2[][2] ={
            {50, 72.5 - (((double)40 / 100) * p[2].ap)},
            {50, 72.5},
            {55, 72.5},
            {55, 72.5 - (((double)40 / 100) * p[2].ap)}
	};

	GLdouble SUP_HP2[][2] ={
            {60, 12.5 - (((double)42.5 / mHP(p[3].type)) * p[3].hp)},
            {60, 12.5},
            {65, 12.5},
            {65, 12.5 - (((double)42.5 / mHP(p[3].type)) * p[3].hp)}
	};

	GLdouble SUP_AP2[][2] ={
            {50, 12.5 - (((double)42.5 / 100) * p[3].ap)},
            {50, 12.5},
            {55, 12.5},
            {55, 12.5 - (((double)42.5 / 100) * p[3].ap)}
	};

	GLdouble DEF_HP2[][2] ={
            {60, -50 - (((double)40 / 800) * p[5].hp)},
            {60, -50},
            {65, -50},
            {65, -50 - (((double)40 / 800) * p[5].hp)}
	};

/*	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, AP_color);
	GLdouble DEF_AP2[][2] ={
	    {50, -50 - (((double)40 / 100) * p[5].ap)},
            {50, -50},
            {55, -50},
            {55, -50 - (((double)40 / 100) * p[5].ap)}
	};
*/
	int i;
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ATK_HP_color);
	glBegin(GL_QUADS);
    	for (i = 0; i < 4; ++i) {
            glVertex2dv(ATK_HP2[i]);
        }	  
  	glEnd();
        
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, AP_color);
        if(p[2].ap == 100)
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, AP_MAX_color);

	glBegin(GL_QUADS);
    	for (i = 0; i < 4; ++i) {
            glVertex2dv(ATK_AP2[i]);
        }	  
  	glEnd();

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, SUP_HP_color);
	glBegin(GL_QUADS);
    	for (i = 0; i < 4; ++i) {
            glVertex2dv(SUP_HP2[i]);
        }	  
  	glEnd();

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, AP_color);
	if(p[3].ap == 100)
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, AP_MAX_color);

	glBegin(GL_QUADS);
    	for (i = 0; i < 4; ++i) {
            glVertex2dv(SUP_AP2[i]);
        }	  
  	glEnd();

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, DEF_HP_color);
	glBegin(GL_QUADS);
    	for (i = 0; i < 4; ++i) {
            glVertex2dv(DEF_HP2[i]);
        }	  
  	glEnd();

/*
	glBegin(GL_QUADS);
    	for (i = 0; i < 4; ++i) {
            glVertex2dv(DEF_AP2[i]);
        }	  
  	glEnd();*/
    }	
}


static void draw3D(void)
{
    //glColorMask(1, 1, 1, 1);  // Alphaも描画する設定
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
    GLfloat aaa[] = { 0.9, 0.9, 0.0, 0.0 };
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, white);
    // 円球を描画する

    glTranslatef(pad.x, pad.y, 0.0f);
    gluCylinder(quadric, 10, 10, 5, 30, 30);
   
    glTranslatef(0.0, 0.0, 5.0f);
    gluDisk(quadric2, 0, 10, 30, 30);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, aaa);
    if (recv_flag == 1)gluCylinder(quadric3, 0, 12, 10, 30, 30);
    glTranslatef(-pad.x, -pad.y, -5.0f);

    //gluDeleteQuadric(quadric);
    //gluDeleteQuadric(quadric2);
    //gluDeleteQuadric(quadric3);

    if (recv_flag == 1) {
        flash++;
        if (flash ==  5) flash_on();
        if (flash >= 10) flash_off();
    }
    else if (flash >= 5)
        flash_off();

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
        {90, 133.1, 98},     //lu
        {90, 122, 125.2},    //ld
        {-90, 122, 125.2},   //rd
        {-90, 133.1, 98}     //ru
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

    /*
    glBegin(GL_QUADS);
    for (i = 0; i < 4; ++i) {
        glVertex3dv(score[face[0][i]]);
        }
    glEnd();
    */
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
    for( x = -xsize; x <= xsize; x += xsize/xnum) {
        glVertex3d( x, -ysize, 0.0 );
        glVertex3d( x, ysize, 0.0 );
    }

    for( y = -ysize; y <= ysize; y += ysize/ynum) {
        glVertex3d( -xsize, y, 0.0 );
        glVertex3d(  xsize, y, 0.0 );
    }
    glEnd();
}

static int Pot(int inSize)
{
    int outSize=1;
    while(outSize<inSize) outSize<<=1;
    return outSize;
}

static void creatTex(char *file, GLuint *tex)
{
    GLuint textures;
    SDL_Surface *imgSrc[2];
    SDL_Rect area;

    //画像読み込み
    imgSrc[0]=SDL_LoadBMP(file);
    if(imgSrc[0]==NULL)SDL_Quit();//読み込まれなかったら終了

    //imgSrc[0]を変換
    imgSrc[1]=SDL_CreateRGBSurface
        (0,
         Pot(imgSrc[0]->w), Pot(imgSrc[0]->h),
         32,
#if SDL_BYTEORDER==SDL_LIL_ENDIAN /* OpenGL RGBA masks */
         0x000000FF, 
         0x0000FF00, 
         0x00FF0000, 
         0xFF000000
#else
         0xFF000000,
         0x00FF0000, 
         0x0000FF00, 
         0x000000FF
#endif
            );
    if(imgSrc[1]==NULL)SDL_Quit();

    area.x=0;  area.y=0;
    area.w=imgSrc[0]->w;
    area.h=imgSrc[0]->h;

    SDL_BlitSurface(imgSrc[0], &area, imgSrc[1], &area);


    //テクスチャ作成
    glGenTextures(1, &textures);
    glBindTexture(GL_TEXTURE_2D, textures);
    //拡大時
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //縮小時
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D
        (
            GL_TEXTURE_2D,//2Dテクスチャ
            0,
            GL_RGB,//GL_RGBA
            imgSrc[1]->w, imgSrc[1]->h,//テクスチャ幅・高さ
            0,//ボーダー
            GL_RGBA,//pixelsの渡すデータ形式
            GL_UNSIGNED_BYTE,//pixelsのデータ型
            imgSrc[1]->pixels
            );
    *tex=textures; if(*tex==0){SDL_Quit();}
    
    int num;//Surface解放
    for(num=0; num<2; num++) SDL_FreeSurface(imgSrc[num]);
}

static void modelD(GLdouble alp)
{
    GLfloat texture_color[] = {1, 1, 1, alp};
    GLfloat texture_color2[] = {0, 0, 1, alp};

    if(cameramode == 0){
    glBindTexture(GL_TEXTURE_2D, texA[p[0].type]);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, texture_color);

    if(p[0].hp == 0)
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, texture_color2);
            
    glBegin(GL_QUADS);
    glTexCoord2i(0, 0);  glVertex2d( 65, 88);
    glTexCoord2i(1, 0);  glVertex2d( 65, 73);
    glTexCoord2i(1, 1);  glVertex2d( 50, 73);
    glTexCoord2i(0, 1);  glVertex2d( 50, 88);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, texA[p[1].type]);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, texture_color);
    if(p[2].hp == 0)
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, texture_color2);

    glBegin(GL_QUADS);
    glTexCoord2i(0, 0);  glVertex2d( 65, 28);
    glTexCoord2i(1, 0);  glVertex2d( 65, 13);
    glTexCoord2i(1, 1);  glVertex2d( 50, 13);
    glTexCoord2i(0, 1);  glVertex2d( 50, 28);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, texA[4]);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, texture_color);
    if(p[4].hp == 0)
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, texture_color2);

    glBegin(GL_QUADS);
    glTexCoord2i(0, 0);  glVertex2d( 65, -34.5);
    glTexCoord2i(1, 0);  glVertex2d( 65, -49.5);
    glTexCoord2i(1, 1);  glVertex2d( 50, -49.5);
    glTexCoord2i(0, 1);  glVertex2d( 50, -34.5);
    glEnd();

    }else if(cameramode == 1){
    glBindTexture(GL_TEXTURE_2D, texA[p[2].type]);
    GLfloat texture_color[] = {1, 1, 1, alp};

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, texture_color);
    if(p[1].hp == 0)
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, texture_color2);

    glBegin(GL_QUADS);
    glTexCoord2i(0, 0);  glVertex2d( 65, 88);
    glTexCoord2i(1, 0);  glVertex2d( 65, 73);
    glTexCoord2i(1, 1);  glVertex2d( 50, 73);
    glTexCoord2i(0, 1);  glVertex2d( 50, 88);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, texA[p[3].type]);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, texture_color);
    if(p[3].hp == 0)
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, texture_color2);

    glBegin(GL_QUADS);
    glTexCoord2i(0, 0);  glVertex2d( 65, 28);
    glTexCoord2i(1, 0);  glVertex2d( 65, 13);
    glTexCoord2i(1, 1);  glVertex2d( 50, 13);
    glTexCoord2i(0, 1);  glVertex2d( 50, 28);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, texA[5]);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, texture_color);
    if(p[5].hp == 0)
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, texture_color2);

    glBegin(GL_QUADS);
    glTexCoord2i(0, 0);  glVertex2d( 65, -34.5);
    glTexCoord2i(1, 0);  glVertex2d( 65, -49.5);
    glTexCoord2i(1, 1);  glVertex2d( 50, -49.5);
    glTexCoord2i(0, 1);  glVertex2d( 50, -34.5);
    glEnd();

    }

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, texture_color);

   if(recv_flag == 100){
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, hassha_img);
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0);  glVertex2d( 0, 40);
	glTexCoord2i(1, 0);  glVertex2d( 0, -40);
	glTexCoord2i(1, 1);  glVertex2d( -20,-40);
   	glTexCoord2i(0, 1);  glVertex2d( -20, 40);
	glEnd();
        glDisable(GL_BLEND);
   }
   
    if(recv_flag == 200){
        glEnable(GL_BLEND);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, cont_img);
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0);  glVertex2d( -10, 50);
	glTexCoord2i(1, 0);  glVertex2d( -10, -50);
	glTexCoord2i(1, 1);  glVertex2d( -30, -50);
   	glTexCoord2i(0, 1);  glVertex2d( -30, 50);
	glEnd();
        glDisable(GL_BLEND);
   }

   if(cameramode == 0){
        glBindTexture(GL_TEXTURE_2D, PointTex[point[0]]);
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0);  glVertex2d( 30, 15);
	glTexCoord2i(1, 0);  glVertex2d( 30, 5);
	glTexCoord2i(1, 1);  glVertex2d( 20, 5);
   	glTexCoord2i(0, 1);  glVertex2d( 20, 15);
	glEnd();

        glBindTexture(GL_TEXTURE_2D, PointTex[point[1]]);
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0);  glVertex2d( 30, -5);
	glTexCoord2i(1, 0);  glVertex2d( 30, -15);
	glTexCoord2i(1, 1);  glVertex2d( 20, -15);
   	glTexCoord2i(0, 1);  glVertex2d( 20, -5);
	glEnd();

   }else if(cameramode == 1){
        glBindTexture(GL_TEXTURE_2D, PointTex[point[1]]);
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0);  glVertex2d( 30, 15);
	glTexCoord2i(1, 0);  glVertex2d( 30, 5);
	glTexCoord2i(1, 1);  glVertex2d( 20, 5);
   	glTexCoord2i(0, 1);  glVertex2d( 20, 15);
	glEnd();

        glBindTexture(GL_TEXTURE_2D, PointTex[point[0]]);
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0);  glVertex2d( 30, -5);
	glTexCoord2i(1, 0);  glVertex2d( 30, -15);
	glTexCoord2i(1, 1);  glVertex2d( 20, -15);
   	glTexCoord2i(0, 1);  glVertex2d( 20, -5);
	glEnd();
   }

   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, texture_color);
    
    alp = 0.9;
    if(recv_flag == 10 || recv_flag == -1){
        glEnable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D, GOAL);
        glBlendFunc(GL_ONE, GL_SRC_COLOR);
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0);  glVertex2d( 10, 20);
	glTexCoord2i(1, 0);  glVertex2d( 10, -20);
	glTexCoord2i(1, 1);  glVertex2d( -10,-20);
   	glTexCoord2i(0, 1);  glVertex2d( -10, 20);
	glEnd();
        glDisable(GL_BLEND);
   }
}

static void modelD_test()
{
    glLoadIdentity();
    glEnable(GL_TEXTURE_2D);//テクスチャON

    static GLdouble alp,vec;
    if     ( (vec==0)&&(alp+=0.01)>1){alp=1; vec=1;}//透過計算
    else if( (vec==1)&&(alp-=0.01)<0){alp=0; vec=0;}
    
    modelD(alp);//modelD
    
    glDisable(GL_TEXTURE_2D);//テクスチャOFF
}

static void flash_on(void)
{
    static GLfloat positionh[4];
    positionh[0] = pad.x;
    positionh[1] = pad.y;
    positionh[2] = 0.0f;
    positionh[3] = 1.0f;

    glLightfv(GL_LIGHT1, GL_POSITION, positionh);
    glEnable(GL_LIGHT1);
}


/*必殺技（致命傷）*/
static void flash_off(void)
{
    glDisable(GL_LIGHT1);
    flash = 0;
}
