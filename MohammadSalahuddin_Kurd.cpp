/*****
CTIS164 - Template Source Program
----------
STUDENT : Mohammad Salahuddin Kurd
SECTION : 3
HOMEWORK: 4

PLEASE ADD GL/ BEFORE THE GLUT LIBRARY (glut.h)

ADDITIONAL FEATURES: BY PRESSING SPACEBAR THE NUMBER OF LIGHT SOURCES WILL INCREASE. THE MAX NUMBER OF LIGHT SOURCES WILL BE 9.
IT IS BEING INDICATED ON SCREEN AS WELL AND MAX MESSAGE APPEAR ON THE SCREEN ONCE THE # OF LIGHT SOURCES ARE MAXIMUM

*****/




#include <glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include "vec.h"

#define WINDOW_WIDTH  750
#define WINDOW_HEIGHT 750

#define TIMER_PERIOD  16 // Period for the timer.
#define TIMER_ON       1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532
/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height
#define RANDOM RAND_MAX

bool activator = true; char tell[5] = "ON";



typedef struct {
	float r, g, b;
} color_t;

typedef struct {
	vec_t   pos;
	color_t color;
	vec_t   vel;
	bool disp;
	char look[5];
} light_t;

typedef struct {
	vec_t pos;
	vec_t N;
} vertex_t;

typedef struct {
	color_t color;
	float angle;
	float speed;
	bool direction;
	float radius;
} planet_t;

#define NUM 10

planet_t plat[NUM];

int num = 4;

light_t light[NUM] =
{
	{ { -100, 0 },{ 1, 0, 0 },{ 6,  2 }, true,"ON"},
	{ { 200, 0 },{ 0, 1, 0 },{ -2, -1 }, true,"ON"},
	{ { -200, 0 },{ 0, 0, 1 },{ 3, 0 }, true,"ON"},
	{ { 0, 0 },{ 1, 1, 0 },{ 0 - 0}, true ,"ON"}

};

color_t mulColor(float k, color_t c) {
	color_t tmp = { k * c.r, k * c.g, k * c.b };
	return tmp;
}

color_t addColor(color_t c1, color_t c2) {
	color_t tmp = { c1.r + c2.r, c1.g + c2.g, c1.b + c2.b };
	return tmp;
}

// To add distance into calculation
// when distance is 0  => its impact is 1.0
// when distance is 350 => impact is 0.0
// Linear impact of distance on light calculation.
double distanceImpact(double d) {
	return fabs(-1.0 / 350.0) * d + 1.0;
}

color_t calculateColor(light_t source, vertex_t v) {
	vec_t L = subV(source.pos, v.pos);
	vec_t uL = unitV(L);
	float factor = dotP(uL, v.N)*distanceImpact(magV(L));
	return mulColor(factor, source.color);
}



//
// to draw circle, center at (x,y)
// radius r
//
void circle(int x, int y, int r)
{
#define PI 3.1415
	float angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r*cos(angle), y + r*sin(angle));
	}
	glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
	float angle;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r*cos(angle), y + r*sin(angle));
	}
	glEnd();
}

void print(int x, int y, char *string, void *font)
{
	int len, i;

	glRasterPos2f(x, y);
	len = (int)strlen(string);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, string[i]);
	}
}

// display text with variables.
void vprint(int x, int y, void *font, char *string, ...)
{
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);

	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, str[i]);
	}
}

void vprint2(int x, int y, float size, char *string, ...) {
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(size, size, 1);

	int len, i;
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
	}
	glPopMatrix();
}

//
// To display onto window using OpenGL commands
//
void drawPlanet(planet_t planet, float radius) {
	float angle;

	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0.3, 0.3, 0.3);
	float x = radius * cos(planet.angle * D2R), y = radius * sin(planet.angle * D2R);
	glVertex2f(x, y);

	for (int j = 0; j <= 100; j++) {
		angle = 2 * PI*j / 100;

		vec_t v = { x + planet.radius*cos(angle), y + planet.radius*sin(angle) };
		vertex_t P = { { v.x, v.y } };

		P.N = unitV(subV({ v.x, v.y }, { x, y }));

		color_t res = { 0, 0, 0 };
		for (int k = 0; k < NUM; k++) {
			if (light[k].disp == true)
				res = addColor(res, calculateColor(light[k], P));
		}

		glColor3f(res.r, res.g, res.b);
		glVertex2f(v.x, v.y);
	}
	glEnd();

}

void board_Display() {
	glColor3f(1, 1, 1);
	vprint(-370, 360, GLUT_BITMAP_9_BY_15, "HOMEWORK 4");
	vprint(-370, 340, GLUT_BITMAP_9_BY_15, "BY SALAHUDDIN");

	vprint(-200, 360, GLUT_BITMAP_9_BY_15, "F1 (SUN): %s", light[3].look);
	vprint(-40, 360, GLUT_BITMAP_9_BY_15, "F2 (BLUE): %s ", light[2].look);
	vprint(-200, 340, GLUT_BITMAP_9_BY_15, "F3 (GREEN): %s", light[1].look);
	vprint(-40, 340, GLUT_BITMAP_9_BY_15, "F4 (RED): %s", light[0].look);
	vprint(120, 360, GLUT_BITMAP_9_BY_15, "F5 (ANNIMATION): %s", tell);
	vprint(120, 340, GLUT_BITMAP_9_BY_15, "F6 : (RESTART)", tell);
	vprint(-200, 320, GLUT_BITMAP_9_BY_15, "PRESS SPACEBAR TO INCREASE # OF LIGHT SOURCES");
	vprint(-200, 300, GLUT_BITMAP_9_BY_15, "# OF LIGHT SOURCES: %d", num - 1);
	if (num == NUM)
		vprint(0, 300, GLUT_BITMAP_9_BY_15, "MAX");



}

void initialize() {

	for (int i = 0; i < 4; i++) {
		plat[i].color = { 150 , 150 , 150 };
		plat[i].direction = rand() & 1;
		plat[i].angle = rand() % 361;
		plat[i].speed = rand() % 2 + 1;
		plat[i].radius = rand() % 6 + 20;
	}
	activator = true; strcpy(tell,"ON");

	light[0] = { { -100, 0 },{ 1, 0, 0 },{ rand() % 5 + 1 *1.0,  2 }, true,"ON" };
	light[1] = { { 200, 0 },{ 0, 1, 0 },{ rand() % 5 + 1 *1.0, -1 }, true,"ON" };
	light[2] = { { -200, 0 }, { 0, 0, 1 }, { rand() % 5 + 1 *1.0 , -2 }, true, "ON" };
	light[3] = { { 0, 0 }, { 1, 1, 1 }, { 0 - 0 }, true, "ON" };

	num = 4;
}

void display() {

	//
	// clear window to black
	//
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	board_Display();
	// light source 
	for (int i = 0; i < num; i++) {
		if (light[i].disp == true)
		{
			glColor3f(light[i].color.r, light[i].color.g, light[i].color.b);
			if (i == 3)
				circle(light[i].pos.x, light[i].pos.y, 30);
			else
				circle(light[i].pos.x, light[i].pos.y, 10);
		}
	}
	for (int x = -250; x <= 250; x++) {
		vertex_t P = { { x, -100 },{ 0, 1 } };
		color_t res = { 0, 0, 0 };
	}
	float radius = 150;
	for (int i = 0; i < 3; i++) {
		
		glColor3f(1, 1, 1);
		circle_wire(0, 0, radius);
		drawPlanet(plat[i], radius);

		radius += 50;
	}


	glutSwapBuffers();

}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);
	if (key == 32 && activator == true && num < 10)
	{
		float r = ((float)rand() / (float)(RANDOM)) + 0;
		float g = ((float)rand() / (float)(RANDOM)) + 0;
		float b = ((float)rand() / (float)(RANDOM)) + 0;
		light[num] = { { 0, 0 },{ r, g, b } };

		light[num].vel = { (double)(rand() % 7) - 2 , (double)(rand() % 7) - 2 };
		light[num].disp = true;
		
		num++;
	}
	// to refresh the window it calls display() function
	glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	// to refresh the window it calls display() function
	glutPostRedisplay();
}
//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
	// Write your codes here.
	if (key == GLUT_KEY_F1)
	{
		if (light[3].disp == true)
		{
			light[3].disp = !light[3].disp;
			strcpy(light[3].look, "OFF");
			light[3].disp = false;

		}
		else if (light[3].disp == false)
		{
			light[3].disp = !light[3].disp;
			strcpy(light[3].look, "ON");
			light[3].disp = true;
		}
	}
	if (key == GLUT_KEY_F2)
	{
		if (light[2].disp == true)
		{
			light[2].disp = !light[2].disp;
			strcpy(light[2].look, "OFF");
			light[2].disp = false;

		}
		else if (light[2].disp == false)
		{
			light[2].disp = !light[2].disp;
			strcpy(light[2].look, "ON");
			light[2].disp = true;
		}
	}
	if (key == GLUT_KEY_F3)
	{
		if (light[1].disp == true)
		{
			light[1].disp = !light[1].disp;
			strcpy(light[1].look, "OFF");
			light[1].disp = false;

		}
		else if (light[1].disp == false)
		{
			light[1].disp = !light[1].disp;
			strcpy(light[1].look, "ON");
			light[1].disp = true;
		}
	}
	if (key == GLUT_KEY_F4)
	{
		if (light[0].disp == true)
		{
			light[0].disp = !light[0].disp;
			strcpy(light[0].look, "OFF");
			light[0].disp = false;

		}
		else if (light[0].disp == false)
		{
			light[0].disp = !light[0].disp;
			strcpy(light[0].look, "ON");
			light[0].disp = true;
		}
	}
	if (key == GLUT_KEY_F5)
	{
		if (activator == true)
		{
			activator = false;
			strcpy(tell, "OFF");
		}
		else if (activator == false)
		{
			activator = true;
			strcpy(tell, "ON");
		}
	}
	if (key == GLUT_KEY_F6)
		initialize();
	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: up = false; break;
	case GLUT_KEY_DOWN: down = false; break;
	case GLUT_KEY_LEFT: left = false; break;
	case GLUT_KEY_RIGHT: right = false; break;
	}
	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)
{
	// Write your codes here.



	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	display(); // refresh window.
}

void onMoveDown(int x, int y) {
	// Write your codes here.



	// to refresh the window it calls display() function   
	glutPostRedisplay();
}

void onMove(int x, int y) {
	// Write your codes here.



	// to refresh the window it calls display() function
	glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	// Write your codes here.
	if (activator ==  true)
	{

	
	for (int i = 0; i < num; i++) {
		light[i].pos = addV(light[i].pos, light[i].vel);

		// Reflection from Walls.
		if (light[i].pos.x > WINDOW_WIDTH / 2 - 10 || light[i].pos.x < -WINDOW_WIDTH / 2 + 10) {
			light[i].vel.x *= -1;
		}

		if (light[i].pos.y > WINDOW_HEIGHT / 2 - 10 || light[i].pos.y < -WINDOW_HEIGHT / 2 + 10) {
			light[i].vel.y *= -1;
		}
	}

	for (int i = 0;i < 3;i++) {
		plat[i].angle += plat[i].speed;


	}
	}


	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()

}
#endif

void Init() {

	// Smoothing shapes
	
	initialize();
}

void main(int argc, char *argv[]) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Planet Reflection Simulation By Salahuddin");

	glutDisplayFunc(display);
	glutReshapeFunc(onResize);

	//
	// keyboard registration
	//
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecialKeyDown);

	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecialKeyUp);

	//
	// mouse registration
	//
	glutMouseFunc(onClick);
	glutMotionFunc(onMoveDown);
	glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
	// timer event
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

	Init();

	glutMainLoop();
}