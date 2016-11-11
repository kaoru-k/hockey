#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <GL/glut.h>

SDL_Rect pack = {0.0, 0.0};
SDL_Rect camera = {0.0, 50.0};

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
	gluPerspective(90.0, (GLdouble) width / (GLdouble) height, 2.0, 200.0);

	// 照明を設定する
	static GLfloat position[] = {-10.0f, 10.0f, 10.0f, 1.0f};
	static GLfloat ambient [] = { 1.0f, 1.0f, 1.0f, 1.0f};
	static GLfloat diffuse [] = { 1.0f, 1.0f, 1.0f, 1.0f};
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
	gluLookAt( camera.x, camera.y, 20.0f,
	           0.0,  0.0f, 0.0f,
	           0.0f,  0.0f, 1.0f);

	// マテリアルを設定する
	GLfloat ambient  [] = { 0.1f, 0.1f, 0.1f, 1.0f};
	GLfloat diffuse  [] = { 1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat specular [] = { 0.0f, 0.0f, 0.0f, 1.0f};
	GLfloat shininess[] = { 0.0f};
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);



	// 円球を描画する
	GLUquadric* quadric = gluNewQuadric();
	GLUquadric* quadric2= gluNewQuadric();
	glTranslatef(pack.x, pack.y, 0.0f);
	gluCylinder(quadric, 10, 10, 5, 30, 30);
	glTranslatef(0.0, 0.0, 5.0f);
	gluDisk(quadric2, 0, 10, 30, 30);
	glTranslatef(-pack.x, -pack.y, -5.0f);
	gluDeleteQuadric(quadric);
	gluDeleteQuadric(quadric2);

	GLdouble vertex[][3] = {
  	{ 100.0, -100.0, 0.0 },
 	{ 101.0, -100.0, 0.0 },
 	{ 101.0, 100.0, 0.0 },
 	{ 100.0, 100.0, 0.0 },
  	{ 100.0, -100.0, 30.0 },
  	{ 101.0, -100.0, 30.0 },
  	{ 101.0, 100.0, 30.0 },
  	{ 100.0, 100.0, 30.0 }
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
    const GLdouble ysize = 100.0f;
    const int xnum = 20;
    const int ynum = 20;
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
		flag = Keyevent();
                draw();
                drawPlane();
                drawAxis();
		SDL_GL_SwapBuffers();
	}
}
