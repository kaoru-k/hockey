#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

bool initializeSDL(int flags) {
	// SDLを初期化する
	if (SDL_Init(flags) < 0) {
		fprintf(stderr, "%s\n", SDL_GetError());
		return false;
	}
	atexit(SDL_Quit);

	return true;
}

bool initializeVideo(int width, int height, int flags) {
	// ビデオモードの設定をする
	if (0 == SDL_SetVideoMode(width, height, 0, flags)) {
		fprintf(stderr, "%s\n", SDL_GetError());
		return false;
	}

	return true;
}

bool initializeOpenGL(int width, int height) {
	if (!initializeVideo(width, height, SDL_OPENGL)) {
		return false;
	}

	// ビューポートを設定する
	glViewport(0, 0, width, height);
	glClearColor( 0.0, 0.0, 0.0, 0.0);
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

	return true;
}

void draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 視点を設定する
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt( -60.0f, 60.0f,-60.0f,
	           0.0f,  0.0f, 0.0f,
	           0.5f, 0.5f, 0.5f);

	// マテリアルを設定する
	GLfloat ambient  [] = { 0.1f, 0.1f, 0.1f, 1.0f};
	GLfloat diffuse  [] = { 1.0f, 0.0f, 0.0f, 1.0f};
	GLfloat specular [] = { 1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat shininess[] = { 0.0f};
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

	// 球を描画する
	GLUquadric* quadric=gluNewQuadric();
	gluCylinder(quadric, 10, 10, 5, 30, 30);
	gluDeleteQuadric(quadric);
}

int main(int argc, char** args) {
	if (!initializeSDL(SDL_INIT_VIDEO)) {
		return 1;
	}

	if (!initializeOpenGL(400, 400)) {
		return 1;
	}

	while (true) {
		// イベントを処理する
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				return 0;
			}
		}

		draw();

		SDL_GL_SwapBuffers();
	}
}
