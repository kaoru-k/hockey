/*************************************
  client_window.c
  クライアントのグラフィック処理
  徳島大学 工学部 知能情報工学科 27班
*************************************/

#include "client.h"
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <GL/glut.h>

/* フィールド */
#define FIELD_H  160   //フィールドの縦幅
#define FIELD_W  100   //フィールドの横幅
#define GOAL_W   0     //ゴールの幅

/* プレイヤー */
#define DEF_Y    150      //ディフェンダーのY座標
#define DEF_W    30       //ディフェンダーの幅
#define SUP_Y    100      //サポーターのY座標
#define SUP_W    10       //サポーターの幅
#define ATK_Y    50       //アタッカーのY座標
#define ATK_W    20       //アタッカーの幅

/* パッド */
#define PAD_R    10       //パッドの半径


float speedx[2] = {0.0, 0.0};	// 自分の操作速度
SDL_Joystick *joystick;         // ジョイスティックを特定・利用するための構造体
PAD pad={3,3,1,1};
SDL_Rect pack = {0.0, 0.0};
SDL_Rect camera = {0.0, 210.0};

static int  initializeSDL(int flags);
static int  initializeVideo(int width, int height, int flags);
static int  initializeOpenGL(int width, int height);
static void draw(void);
static void drawAxis(void);
static void drawPlane(void);
int draw_field(void);

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

static void draw(void)
{
    ugoki();
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
    

    GLfloat white[] = { 1.0, 1.0, 0.49, 1.0 };
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
								camera.x -= 1;
								break;
						case SDLK_LEFT:
								camera.x += 1;
								break;
						case SDLK_UP:
								camera.y -= 1;
								break;
						case SDLK_DOWN:
								camera.y += 1;
								break;
						case SDLK_d:
								speedx[0] = -3;
								break;
						case SDLK_a:
								speedx[0] = 3;
								break;
						case SDLK_w:
						case SDLK_s:
								speedx[0] = 0;
								break;
                				default:
								break;
					   }
			case SDL_JOYAXISMOTION:
				printf("The axis ID of the operated key is %d.\n",event.jaxis.axis);	// 操作された方向キーの方向軸を表示（0：アナログキー，1：アナログキー，2：方向キー左右方向，3：方向キー上下方向）
				if(event.jaxis.axis==0){
					printf("--- Analog-Direction Key: ?? Axis\n");
					if(event.jaxis.value < 0)
						speedx[1] += 3;
					else if(event.jaxis.value >  0)
						speedx[1] -= 3;
					else
						speedx[1] = 0.0;
				}
				else if(event.jaxis.axis==1){
					printf("--- Anolag-Direction Key: ?? Axis\n");
					/*else
						cursor_axis.y = 0;*/
				}
				else if(event.jaxis.axis==2){
					printf("--- Four-Direction Key: Horizontal Axis\n");
					
				}
				else if(event.jaxis.axis==3){
					printf("--- Four-Direction Key: Vertical Axis\n");
				}						
				printf("--- The axis value of the operated key is %d.\n",event.jaxis.value);	// ジョイスティックの操作された方向キーの値を表示（-32767（真左，真上）～32767（真右，真下））
				break;
		// ジョイスティックのボタンが押された時
		/*case SDL_JOYBUTTONDOWN:
			switch(event.jbutton.button){
				case 4:
				case 6: if(cursormode == 1)
						cursormode = 0;
					break;
				case 5:
				case 7: if(cursormode == 0)
						cursormode = 1;
					break;
				case 0 :
				case 1 :
				case 2 :
				case 3 :	if(prog == 0){
							prog = 1;
							textmode = 1;
						}

						if(clear == 1){
							prog++;
							textmode = 1;
							clear = 0;
						}

						if(chara_rect.y + 90 == GROUND){
						if(chara_rect.x <= cursor_rect.x + 25 && cursor_rect.x + 25 <= chara_rect.x + 92 && chara_rect.y +  90 <= cursor_rect.y + 25 && cursor_rect.y + 25 <= chara_rect.y +  90 + 25 && cursormode == 0)
							accely = 8.0;
						if(chara_rect.x <= cursor_rect.x + 25 && cursor_rect.x + 25 <= chara_rect.x + 92 && chara_rect.y <= cursor_rect.y + 25 && cursor_rect.y + 25 <= chara_rect.y +  90  && cursormode == 0)
							SDL_Delay(1000);
					 }	 
					 break;
				case 10 :
				case 11 : return 0;
					  break;
				default : break;
			}
			printf("The ID of the pressed button is %d.\n", event.jbutton.button);	// 押されたボタンのIDを表示（0から）
			// ボタンIDに応じた処理
			if(event.jbutton.button==0){
				printf("--- You pressed a button on the joystick.\n");
			}
			break;
			// ボタンが離された時
		case SDL_JOYBUTTONUP:
			printf("The ID of the released button is %d.\n", event.jbutton.button);	// 離されたボタンのIDを表示（0から）
			// ボタンIDに応じた処理
			if(event.jbutton.button==0){
				printf("--- You released a button on the joystick.\n");
			}
			break;*/
		}
		
	}

	if(FIELD_W > p[0].x + speedx[0] + ATK_W && p[0].x + speedx[0] - ATK_W > -FIELD_W)
		p[0].x += speedx[0];
	if(FIELD_W > p[1].x + speedx[1] + ATK_W && p[1].x + speedx[1] - ATK_W > -FIELD_W)
		p[1].x += speedx[1];

	return 1;
}

int draw_field(void) {
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
