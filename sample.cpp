#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define _USE_MATH_DEFINES
#include <math.h>

#ifndef F_PI
#define F_PI		((float)(M_PI))
#define F_2_PI		((float)(2.f*F_PI))
#define F_PI_2		((float)(F_PI/2.f))
#endif


#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include "glew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include "glut.h"

#define GLM_FORCE_RADIANS
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat3x3.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"



//	This is a sample OpenGL / GLUT program
//
//	The objective is to draw a 3d object and change the color of the axes
//		with a glut menu
//
//	The left mouse button does rotation
//	The middle mouse button does scaling
//	The user interface allows:
//		1. The axes to be turned on and off
//		2. The color of the axes to be changed
//		3. Debugging to be turned on and off
//		4. Depth cueing to be turned on and off
//		5. The projection to be changed
//		6. The transformations to be reset
//		7. The program to quit
//
//	Author:			Joe Graphics

// title of these windows:

const char *WINDOWTITLE = "OpenGL / GLUT Sample -- Joe Graphics";
const char *GLUITITLE   = "User Interface Window";

// what the glui package defines as true and false:

const int GLUITRUE  = true;
const int GLUIFALSE = false;

// the escape key:

const int ESCAPE = 0x1b;

// initial window size:

const int INIT_WINDOW_SIZE = 600;

// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = 1.f;
const float SCLFACT = 0.005f;

// minimum allowable scale factor:

const float MINSCALE = 0.05f;

// scroll wheel button values:

const int SCROLL_WHEEL_UP   = 3;
const int SCROLL_WHEEL_DOWN = 4;

// equivalent mouse movement when we click the scroll wheel:

const float SCROLL_WHEEL_CLICK_FACTOR = 5.f;

// active mouse buttons (or them together):

const int LEFT   = 4;
const int MIDDLE = 2;
const int RIGHT  = 1;

// which projection:

enum Projections
{
	ORTHO,
	PERSP
};

// which button:

enum ButtonVals
{
	RESET,
	QUIT
};

// window background color (rgba):

const GLfloat BACKCOLOR[ ] = { 0., 0., 0., 1. };

// line width for the axes:

const GLfloat AXES_WIDTH   = 3.;

// the color numbers:
// this order must match the radio button order, which must match the order of the color names,
// 	which must match the order of the color RGB values

enum Colors
{
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA
};

char * ColorNames[ ] =
{
	(char *)"Red",
	(char*)"Yellow",
	(char*)"Green",
	(char*)"Cyan",
	(char*)"Blue",
	(char*)"Magenta"
};

// the color definitions:
// this order must match the menu order

const GLfloat Colors[ ][3] = 
{
	{ 1., 0., 0. },		// red
	{ 1., 1., 0. },		// yellow
	{ 0., 1., 0. },		// green
	{ 0., 1., 1. },		// cyan
	{ 0., 0., 1. },		// blue
	{ 1., 0., 1. },		// magenta
};

// for lighting:

const float	WHITE[ ] = { 1.,1.,1.,1. };

// for animation:

const int MS_PER_CYCLE = 10000;		// 10000 milliseconds = 10 seconds

// non-constant global variables:

int		ActiveButton;			// current button that is down
GLuint	AxesList;				// list to hold the axes
int		AxesOn;					// != 0 means to draw the axes
int		DebugOn;				// != 0 means to print debugging info
bool	Freeze;
int		MainWindow;				// window id for main graphics window
int		NowColor;				// index into Colors[ ]
int		NowProjection;			// ORTHO or PERSP
float	Scale;					// scaling factor
float	Time;					// used for animation, this has a value between 0. and 1.
int		Xmouse, Ymouse;			// mouse values
float	Xrot, Yrot;				// rotation angles in degrees

int		SphereList;
GLuint	BoxList;				// for the play button

GLuint	instrList1;				// for displaying level 1 instructions
GLuint	contrList1;				// for displaying level 1 controls
GLuint	keypad1;				// for pressing 1 on keypad
GLuint	imageSquareList;		// before image

GLuint	keypadPound;
GLuint	keypadStar;
GLuint	keypadScreen;

// scene/level variables
int currentLevel;				// for different displays
int lockPin[4];
int lockGuess[4];
int currentTumbler;				// which digit is being selected, 5 = enter
// picking variables
const int numPickableObjects = 100; // number of clickable objects
float pickingRGB[3][numPickableObjects]; // r,g,b values for each object for picking
//int pickedObject[numPickableObjects];	// 0 for not picked, 1 for picked
int pickingMode;				// 0 for regular shader, 1 for picking shader
int currentObjectPicked;		// ID of the currently selected object

// pickable objects
// 0 is background
// 1 is Play button (BoxList)
// 2 is InstrList1
// 3 is ContrList1
// 4-14 are keypad buttons



// function prototypes:

void	Animate( );
void	Display( );
void	DoAxesMenu( int );
void	DoColorMenu( int );
void	DoDebugMenu( int );
void	DoMainMenu( int );
void	DoProjectMenu( int );
void	DoRasterString( float, float, float, char * );
void	DoRasterString10(float x, float y, float z, char* s);
void	DoRasterString18(float x, float y, float z, char* s);
void	DoStrokeString( float, float, float, float, char * );
float	ElapsedSeconds( );
void	InitGraphics( );
void	InitLists( );
void	InitMenus( );
void	Keyboard( unsigned char, int, int );
void	MouseButton( int, int, int, int );
void	MouseMotion( int, int );
void	Reset( );
void	Resize( int, int );
void	Visibility( int );

void	Axes( float );
void	HsvRgb( float[3], float [3] );
void	Cross(float[3], float[3], float[3]);
float	Dot(float [3], float [3]);
float	Unit(float [3], float [3]);
float	Unit(float [3]);

// custom function prototypes:

void	initPicking();
int		whichObject();

// custom function definitions:
void
initPicking()
{
	pickingMode = 0;
	for (int i = 0; i < numPickableObjects; i++)
	{
		int r = (i & 0x000000FF);
		int g = (i & 0x0000FF00);
		int b = (i & 0x00FF0000);

		pickingRGB[0][i] = r / 255.0f;
		pickingRGB[1][i] = g / 255.0f;
		pickingRGB[2][i] = b / 255.0f;
		//pickingRGB[0][i] = r;
		//pickingRGB[1][i] = g;
		//pickingRGB[2][i] = b;

		//pickedObject[i] = 0;
	}
}

int
whichObject()
{
	// now read the sampled pixel for color using current mouse position
	unsigned char data[4]; // for void * storage from glReadPixels
	glReadPixels(Xmouse, Ymouse, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

	//convert color from data to a objColorID
	int objColorID = data[0] + data[1] * 256 + data[2] * 256 * 256;

	//pickedObject[objColorID] = 1;
	//pickedObject[currentObjectPicked] = 0;

	currentObjectPicked = objColorID;

	return objColorID;
}




// utility to create an array from 3 separate values:

float *
Array3( float a, float b, float c )
{
	static float array[4];

	array[0] = a;
	array[1] = b;
	array[2] = c;
	array[3] = 1.;
	return array;
}

// utility to create an array from a multiplier and an array:

float *
MulArray3( float factor, float array0[ ] )
{
	static float array[4];

	array[0] = factor * array0[0];
	array[1] = factor * array0[1];
	array[2] = factor * array0[2];
	array[3] = 1.;
	return array;
}


float *
MulArray3(float factor, float a, float b, float c )
{
	static float array[4];

	float* abc = Array3(a, b, c);
	array[0] = factor * abc[0];
	array[1] = factor * abc[1];
	array[2] = factor * abc[2];
	array[3] = 1.;
	return array;
}

// these are here for when you need them -- just uncomment the ones you need:

//#include "setmaterial.cpp"
//#include "setlight.cpp"
#include "osusphere.cpp"
//#include "osucone.cpp"
//#include "osutorus.cpp"
//#include "bmptotexture.cpp"
//#include "loadobjfile.cpp"
//#include "keytime.cpp"
#include "glslprogram.cpp"

float NowS0, NowT0, NowD;
GLSLProgram Pattern;
glm::vec4 pickingColorVec4;
GLSLProgram PatternPicking;
float nowT;
GLSLProgram ImageDifference;
float nowTBlur;
GLSLProgram ImageBlur;
float nowTBrightness;
GLSLProgram ImageBrightness;


// main program:

int
main( int argc, char *argv[ ] )
{
	currentLevel = 0;
	lockPin[0] = 4;
	lockPin[1] = 3;
	lockPin[2] = 9;
	lockPin[3] = 1;
	lockGuess[0] = 0;
	lockGuess[1] = 0;
	lockGuess[2] = 0;
	lockGuess[3] = 0;
	currentTumbler = 0;

	// init picking program
	initPicking();

	// turn on the glut package:
	// (do this before checking argc and argv since glutInit might
	// pull some command line arguments out)

	glutInit(&argc, argv);

	// setup all the graphics stuff:
	// this needs to be called once and only once!

	InitGraphics( );

	// create the display lists that **will not change**:

	InitLists( );

	// init all the global variables used by Display( ):
	// this will also post a redisplay

	Reset( );

	// setup all the user interface stuff:

	InitMenus( );

	// draw the scene once and wait for some interaction:
	// (this will never return)

	glutSetWindow( MainWindow );
	glutMainLoop( );

	// glutMainLoop( ) never actually returns
	// the following line is here to make the compiler happy:

	return 0;
}


// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
//
// this is typically where animation parameters are set
//
// do not call Display( ) from here -- let glutPostRedisplay( ) do it

void
Animate( )
{
	// put animation stuff in here -- change some global variables for Display( ) to find:

	int ms = glutGet(GLUT_ELAPSED_TIME);
	ms %= MS_PER_CYCLE;							// makes the value of ms between 0 and MS_PER_CYCLE-1
	Time = (float)ms / (float)MS_PER_CYCLE;		// makes the value of Time between 0. and slightly less than 1.

	// for example, if you wanted to spin an object in Display( ), you might call: glRotatef( 360.f*Time,   0., 1., 0. );

	// force a call to Display( ) next time it is convenient:

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// draw the complete scene:

void
Display()
{
	// set which window we want to do the graphics into:
	glutSetWindow(MainWindow);

	// erase the background:
	glDrawBuffer(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	// specify shading to be flat:

	glShadeModel(GL_FLAT);

	// set the viewport to be a square centered in the window:

	GLsizei vx = glutGet(GLUT_WINDOW_WIDTH);
	GLsizei vy = glutGet(GLUT_WINDOW_HEIGHT);
	GLsizei v = vx < vy ? vx : vy;			// minimum dimension
	GLint xl = (vx - v) / 2;
	GLint yb = (vy - v) / 2;
	glViewport(xl, yb, v, v);

	// set the viewing volume:
	// remember that the Z clipping  values are given as DISTANCES IN FRONT OF THE EYE
	// USE gluOrtho2D( ) IF YOU ARE DOING 2D !

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (NowProjection == ORTHO)
		glOrtho(-2.f, 2.f, -2.f, 2.f, 0.1f, 1000.f);
	else
		glOrtho(-2.f, 2.f, -2.f, 2.f, 0.1f, 1000.f);
	//gluPerspective( 70.f, 1.f,	0.1f, 1000.f );

// place the objects into the scene:

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// set the eye position, look-at position, and up-vector:

	gluLookAt(0.f, 0.f, 3.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f);

	// rotate the scene:

	// comment out to lock camera rotation
	glRotatef( (GLfloat)Yrot, 0.f, 1.f, 0.f );
	glRotatef( (GLfloat)Xrot, 1.f, 0.f, 0.f );

	// uniformly scale the scene:

	if (Scale < MINSCALE)
		Scale = MINSCALE;
	glScalef((GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale);

	// possibly draw the axes:

	if (AxesOn != 0)
	{
		glColor3fv(&Colors[NowColor][0]);
		glCallList(AxesList);
	}

	// since we are using glScalef( ), be sure the normals get unitized:

	glEnable(GL_NORMALIZE);

	//glEnable(GL_LIGHTING);

	//SetPointLight(GL_LIGHT0, 0.f, 0.f, -3.f, 0.f, 0.f, 0.f);

	// draw the box object by calling up its display list:

	if (currentLevel == 0)
	{
		if (pickingMode == 1)
		{
			glClearColor(0.f, 0.f, 0.f, 1.f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			PatternPicking.Use();
			//0: background
			//pickingColorVec4 = glm::vec4(pickingRGB[0][0], pickingRGB[1][0], pickingRGB[2][0], 1.);
			// couldn't find below overloaded function definition for some reason
			//PatternPicking.SetUniformVariable( (char*)"PickingColor", pickingColorVec4);
			//PatternPicking.SetUniformVariable((char*)"PickingColor", pickingRGB[0][0], pickingRGB[1][0], pickingRGB[2][0], 1.);
			// Now draw background

			// 2: Play Button (BoxList)
			PatternPicking.SetUniformVariable((char*)"PickingColor", pickingRGB[0][1], pickingRGB[1][1], pickingRGB[2][1], 1.);
			glCallList(BoxList);
			//2 and so on

			glFlush();
			glFinish();

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			int objectPicked = whichObject();

			PatternPicking.UnUse();

			pickingMode = 0;
		}
		glCallList(BoxList);

		if (currentObjectPicked == 1)
		{
			//has hit play, move on to level 1
			currentLevel = 1;
		}

	}
	else if (currentLevel == 1)
	{
		// difference level
		if (pickingMode == 1)
		{
			glClearColor(0.f, 0.f, 0.f, 1.f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			PatternPicking.Use();
			//0: background
			//pickingColorVec4 = glm::vec4(pickingRGB[0][0], pickingRGB[1][0], pickingRGB[2][0], 1.);
			// couldn't find below overloaded function definition for some reason
			//PatternPicking.SetUniformVariable( (char*)"PickingColor", pickingColorVec4);
			//PatternPicking.SetUniformVariable((char*)"PickingColor", pickingRGB[0][0], pickingRGB[1][0], pickingRGB[2][0], 1.);
			// Now draw background

			// 2: Instructions (instrList1)
			PatternPicking.SetUniformVariable((char*)"PickingColor", pickingRGB[0][2], pickingRGB[1][2], pickingRGB[2][2], 1.);
			glTranslatef(-1.5f, 1.7f, 0.f);
			glCallList(instrList1);
			//glMatrixMode(GL_MODELVIEW);
			//glLoadIdentity();
			//3: Controls (contrList1)
			PatternPicking.SetUniformVariable((char*)"PickingColor", pickingRGB[0][3], pickingRGB[1][3], pickingRGB[2][3], 1.);
			glTranslatef(1.5f, 0.f, 0.f);
			glCallList(contrList1);
			//4: Keypad 1 (all keypad1)
			PatternPicking.SetUniformVariable((char*)"PickingColor", pickingRGB[0][4], pickingRGB[1][4], pickingRGB[2][4], 1.);
			glTranslatef(1.1f, -0.8f, 0.f);
			glCallList(keypad1);
			//5: Keypad 2 (all keypad1)
			PatternPicking.SetUniformVariable((char*)"PickingColor", pickingRGB[0][5], pickingRGB[1][5], pickingRGB[2][5], 1.);
			glTranslatef(0.3f, 0.f, 0.f);
			glCallList(keypad1);
			//6: Keypad 3 (all keypad1)
			PatternPicking.SetUniformVariable((char*)"PickingColor", pickingRGB[0][6], pickingRGB[1][6], pickingRGB[2][6], 1.);
			glTranslatef(0.3f, 0.f, 0.f);
			glCallList(keypad1);
			//7: Keypad 4 (all keypad1)
			PatternPicking.SetUniformVariable((char*)"PickingColor", pickingRGB[0][7], pickingRGB[1][7], pickingRGB[2][7], 1.);
			glTranslatef(-0.6f, 0.3f, 0.f);
			glCallList(keypad1);
			//8: Keypad 5 (all keypad1)
			PatternPicking.SetUniformVariable((char*)"PickingColor", pickingRGB[0][8], pickingRGB[1][8], pickingRGB[2][8], 1.);
			glTranslatef(0.3f, 0.f, 0.f);
			glCallList(keypad1);
			//9: Keypad 6 (all keypad1)
			PatternPicking.SetUniformVariable((char*)"PickingColor", pickingRGB[0][9], pickingRGB[1][9], pickingRGB[2][9], 1.);
			glTranslatef(0.3f, 0.f, 0.f);
			glCallList(keypad1);
			//10: Keypad 7 (all keypad1)
			PatternPicking.SetUniformVariable((char*)"PickingColor", pickingRGB[0][10], pickingRGB[1][10], pickingRGB[2][10], 1.);
			glTranslatef(-0.6f, 0.3f, 0.f);
			glCallList(keypad1);
			//11: Keypad 8 (all keypad1)
			PatternPicking.SetUniformVariable((char*)"PickingColor", pickingRGB[0][11], pickingRGB[1][11], pickingRGB[2][11], 1.);
			glTranslatef(0.3f, 0.f, 0.f);
			glCallList(keypad1);
			//12: Keypad 9 (all keypad1)
			PatternPicking.SetUniformVariable((char*)"PickingColor", pickingRGB[0][12], pickingRGB[1][12], pickingRGB[2][12], 1.);
			glTranslatef(0.3f, 0.f, 0.f);
			glCallList(keypad1);
			//13: Keypad 0 (all keypad1)
			PatternPicking.SetUniformVariable((char*)"PickingColor", pickingRGB[0][13], pickingRGB[1][13], pickingRGB[2][13], 1.);
			glTranslatef(-0.3f, 0.3f, 0.f);
			glCallList(keypad1);
			//14: Keypad * Delete (SphereList)
			PatternPicking.SetUniformVariable((char*)"PickingColor", pickingRGB[0][14], pickingRGB[1][14], pickingRGB[2][14], 1.);
			glTranslatef(-0.3f, 0.f, 0.f);
			glCallList(SphereList);
			//15: Keypad # Enter (SphereList)
			PatternPicking.SetUniformVariable((char*)"PickingColor", pickingRGB[0][15], pickingRGB[1][15], pickingRGB[2][15], 1.);
			glTranslatef(0.6f, 0.f, 0.f);
			glCallList(SphereList);
			//16: Before Photo (imageSquareList)
			PatternPicking.SetUniformVariable((char*)"PickingColor", pickingRGB[0][16], pickingRGB[1][16], pickingRGB[2][16], 1.);
			glTranslatef(-2.8f, -2.9f, 0.f);
			glCallList(imageSquareList);
			//
			glFlush();
			glFinish();

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			int objectPicked = whichObject();

			PatternPicking.UnUse();

			pickingMode = 0;

			if (currentObjectPicked >= 4 && currentObjectPicked <= 13)
			{
				// digit was entered
				//lockGuess[currentTumbler] = currentObjectPicked - 
				if (currentTumbler >= 4)
				{
					// no more room for input
				}
				else
				{
					switch (currentObjectPicked)
					{
					case 4:
						lockGuess[currentTumbler] = 1;
						break;
					case 5:
						lockGuess[currentTumbler] = 2;
						break;
					case 6:
						lockGuess[currentTumbler] = 3;
						break;
					case 7:
						lockGuess[currentTumbler] = 4;
						break;
					case 8:
						lockGuess[currentTumbler] = 5;
						break;
					case 9:
						lockGuess[currentTumbler] = 6;
						break;
					case 10:
						lockGuess[currentTumbler] = 7;
						break;
					case 11:
						lockGuess[currentTumbler] = 8;
						break;
					case 12:
						lockGuess[currentTumbler] = 9;
						break;
					case 13:
						lockGuess[currentTumbler] = 0;
						break;
					}
					currentTumbler += 1;
				}
			}
			else if (currentObjectPicked == 14)
			{
				lockGuess[0] = 0;
				lockGuess[1] = 0;
				lockGuess[2] = 0;
				lockGuess[3] = 0;
				currentTumbler = 0;
			}
			else if (currentObjectPicked == 15)
			{
				int unlocked = 1; // 1 by default, set to 0 if any digits wrong

				for (int i = 0; i < 4; i++)
				{
					if (lockGuess[i] != lockPin[i])
						unlocked = 0;
				}
				if (unlocked)
					currentLevel = 2;
				else
				{
					lockGuess[0] = 0;
					lockGuess[1] = 0;
					lockGuess[2] = 0;
					lockGuess[3] = 0;
					currentTumbler = 0;
				}
			}
			//glTranslatef(1.5f, 1.7f, 0.f);
		}
		else
		{
			// Draw Regular scene
			Pattern.Use();

			//// set the uniform variables that will change over time:

			NowS0 = 0.0f;
			NowT0 = 0.0f;
			NowD = 0.2f + 0.1f * sinf(2.f * F_PI * Time);
			Pattern.SetUniformVariable((char*)"uS0", NowS0);
			Pattern.SetUniformVariable((char*)"uT0", NowT0);
			Pattern.SetUniformVariable((char*)"uD", NowD);

			////glCallList( SphereList );
			glTranslatef(-1.5f, -1.7f, 0.f);
			glCallList(instrList1);

			glTranslatef(1.5f, 0.f, 0.f);
			glCallList(contrList1);

			glTranslatef(1.1f, 0.8f, 0.f);
			glCallList(keypad1);

			glTranslatef(0.3f, 0.f, 0.f);
			glCallList(keypad1);

			glTranslatef(0.3f, 0.f, 0.f);
			glCallList(keypad1);

			glTranslatef(-0.6f, -0.3f, 0.f);
			glCallList(keypad1);

			glTranslatef(0.3f, 0.f, 0.f);
			glCallList(keypad1);

			glTranslatef(0.3f, 0.f, 0.f);
			glCallList(keypad1);

			glTranslatef(-0.6f, -0.3f, 0.f);
			glCallList(keypad1);

			glTranslatef(0.3f, 0.f, 0.f);
			glCallList(keypad1);

			glTranslatef(0.3f, 0.f, 0.f);
			glCallList(keypad1);

			glTranslatef(-0.3f, -0.3f, 0.f);
			glCallList(keypad1);

			glTranslatef(-0.3f, 0.f, 0.f);
			glCallList(SphereList);

			glTranslatef(0.6f, 0.f, 0.f);
			glCallList(SphereList);

			glFlush();
			glFinish();

			Pattern.UnUse();       // Pattern.Use(0);  also works


			ImageDifference.Use();
			//GLint temp = glGetUniformLocation(ImageDifference, "t");
			//glEnable(GL_TEXTURE_2D);
			//glActiveTexture(GL_TEXTURE0 +

			nowT = 0.5f;
			ImageDifference.SetUniformVariable((char*)"t", nowT);
			//int uTexUnitB = 5;
			//int uTexUnitA = 6;

			//glActiveTexture(GL_TEXTURE0 + uTexUnitB);
			//glBindTexture(GL_TEXTURE_2D, imageSquareList);
			//glActiveTexture(GL_TEXTURE0 + uTexUnitA);
			//glBindTexture(GL_TEXTURE_2D, imageSquareList);
			// 
			//glTranslatef(-2.8f, 2.9f, 0.f);
			//glScalef(2.f, 2.f, 2.f);
			glTranslatef(-2.8f, 2.9f, 0.f);
			glCallList(imageSquareList);

			glFlush();
			glFinish();

			ImageDifference.UnUse();
			//Pattern.UnUse();
			//glTranslatef(-1.5f, -1.7f, 0.f);
			//glCallList(instrList1);
		}


	}
	else if (currentLevel == 2)
	{
		// Image Blur Level
		if (pickingMode == 1)
		{
			glClearColor(0.f, 0.f, 0.f, 1.f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			PatternPicking.Use();
			//0: background
			//pickingColorVec4 = glm::vec4(pickingRGB[0][0], pickingRGB[1][0], pickingRGB[2][0], 1.);
			// couldn't find below overloaded function definition for some reason
			//PatternPicking.SetUniformVariable( (char*)"PickingColor", pickingColorVec4);
			//PatternPicking.SetUniformVariable((char*)"PickingColor", pickingRGB[0][0], pickingRGB[1][0], pickingRGB[2][0], 1.);
			// Now draw background

			//17: Before Photo (imageSquareList)
			PatternPicking.SetUniformVariable((char*)"PickingColor", pickingRGB[0][17], pickingRGB[1][17], pickingRGB[2][17], 1.);
			//glTranslatef(-2.8f, -2.9f, 0.f);
			glCallList(imageSquareList);
			// 2: Instructions (instrList1)
			PatternPicking.SetUniformVariable((char*)"PickingColor", pickingRGB[0][2], pickingRGB[1][2], pickingRGB[2][2], 1.);
			glTranslatef(-1.5f, 1.7f, 0.f);
			glCallList(instrList1);


			//
			glFlush();
			glFinish();

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			int objectPicked = whichObject();

			PatternPicking.UnUse();

			pickingMode = 0;

			if (currentObjectPicked == 17)
			{
				currentLevel = 3;
			}
		}
		else
		{
			// Draw Regular scene
			ImageBlur.Use();

			//// set the uniform variables that will change over time:

			//NowS0 = 0.5f;
			//NowT0 = 0.5f;
			//NowD = 0.2f + 0.1f * sinf(2.f * F_PI * Time);
			//Pattern.SetUniformVariable((char*)"uS0", NowS0);
			//Pattern.SetUniformVariable((char*)"uT0", NowT0);
			//Pattern.SetUniformVariable((char*)"uD", NowD);
			nowTBlur = 1.f;
			ImageBlur.SetUniformVariable((char*)"t", nowTBlur);

			////glCallList( SphereList );
			glCallList(imageSquareList);

			glFlush();
			glFinish();

			ImageBlur.UnUse();       // Pattern.Use(0);  also works


			//ImageDifference.Use();


			//nowT = 0.5f;
			//ImageDifference.SetUniformVariable((char*)"t", nowT);


			//glScalef(2.f, 2.f, 2.f);
			//glTranslatef(-1.4f, 1.5f, 0.f);
			//glCallList(imageSquareList);

			//glFlush();
			//glFinish();

			//ImageDifference.UnUse();
		}
	}
	else if (currentLevel == 3)
	{
		// Image Brightness Level
		if (pickingMode == 1)
		{
			glClearColor(0.f, 0.f, 0.f, 1.f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			PatternPicking.Use();
			//0: background
			//pickingColorVec4 = glm::vec4(pickingRGB[0][0], pickingRGB[1][0], pickingRGB[2][0], 1.);
			// couldn't find below overloaded function definition for some reason
			//PatternPicking.SetUniformVariable( (char*)"PickingColor", pickingColorVec4);
			//PatternPicking.SetUniformVariable((char*)"PickingColor", pickingRGB[0][0], pickingRGB[1][0], pickingRGB[2][0], 1.);
			// Now draw background

			//18: Before Photo (imageSquareList)
			PatternPicking.SetUniformVariable((char*)"PickingColor", pickingRGB[0][18], pickingRGB[1][18], pickingRGB[2][18], 1.);
			//glTranslatef(-2.8f, -2.9f, 0.f);
			glCallList(imageSquareList);
			// 2: Instructions (instrList1)
			PatternPicking.SetUniformVariable((char*)"PickingColor", pickingRGB[0][2], pickingRGB[1][2], pickingRGB[2][2], 1.);
			glTranslatef(-1.5f, 1.7f, 0.f);
			glCallList(instrList1);


			//
			glFlush();
			glFinish();

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			int objectPicked = whichObject();

			PatternPicking.UnUse();

			pickingMode = 0;

			//if (currentObjectPicked == 17)
			//{
			//	currentLevel = 3;
			//}
		}
		else
		{
			// Draw Regular scene
			ImageBrightness.Use();

			//// set the uniform variables that will change over time:

			//NowS0 = 0.5f;
			//NowT0 = 0.5f;
			//NowD = 0.2f + 0.1f * sinf(2.f * F_PI * Time);
			//Pattern.SetUniformVariable((char*)"uS0", NowS0);
			//Pattern.SetUniformVariable((char*)"uT0", NowT0);
			//Pattern.SetUniformVariable((char*)"uD", NowD);
			nowTBrightness = 1.f;
			ImageBrightness.SetUniformVariable((char*)"t", nowTBlur);

			////glCallList( SphereList );
			glCallList(imageSquareList);

			glFlush();
			glFinish();

			ImageBrightness.UnUse();       // Pattern.Use(0);  also works


			//ImageDifference.Use();


			//nowT = 0.5f;
			//ImageDifference.SetUniformVariable((char*)"t", nowT);


			//glScalef(2.f, 2.f, 2.f);
			//glTranslatef(-1.4f, 1.5f, 0.f);
			//glCallList(imageSquareList);

			//glFlush();
			//glFinish();

			//ImageDifference.UnUse();
		}
	}
	//glDisable(GL_LIGHTING);
	////////PICKING SHADER/////////////
	//if (pickingMode == 1)
	//{
	//	PatternPicking.Use();
	//	//0: background
	//	pickingColorVec4 = glm::vec4(pickingRGB[0][0], pickingRGB[1][0], pickingRGB[2][0], 1.);
	//	// couldn't find below overloaded function definition for some reason
	//	//PatternPicking.SetUniformVariable( (char*)"PickingColor", pickingColorVec4);
	//	PatternPicking.SetUniformVariable((char*)"PickingColor", pickingRGB[0][0], pickingRGB[1][0], pickingRGB[2][0], 1.);
	//	// Now draw background
	//	
	//	// 1: BoxList (Play Button)
	//	PatternPicking.SetUniformVariable((char*)"PickingColor", pickingRGB[0][1], pickingRGB[1][1], pickingRGB[2][1], 1.);
	//	glCallList(BoxList);
	//	////1: SphereList
	//	//PatternPicking.SetUniformVariable((char*)"PickingColor", pickingRGB[0][1], pickingRGB[1][1], pickingRGB[2][1], 1.);
	//	//glCallList(SphereList);
	//	//2 and so on

	//	glFlush();
	//	glFinish();

	//	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	//	int objectPicked = whichObject();

	//	PatternPicking.UnUse();

	//	pickingMode = 0;
	//}
	


	///////////////
	
	//Pattern.Use( );

	//// set the uniform variables that will change over time:

	//NowS0 = 0.5f;
	//NowT0 = 0.5f;
	//NowD  = 0.2f + 0.1f*sinf(2.f*F_PI*Time);
	//Pattern.SetUniformVariable( (char *)"uS0", NowS0 );
	//Pattern.SetUniformVariable( (char *)"uT0", NowT0 );
	//Pattern.SetUniformVariable( (char *)"uD" , NowD  );

	////glCallList( SphereList );
	//glTranslatef(-1.5f, -1.7f, 0.f);
	//glCallList(BoxList);

	//Pattern.UnUse( );       // Pattern.Use(0);  also works


	// draw some gratuitous text that just rotates on top of the scene:
	// i commented out the actual text-drawing calls -- put them back in if you have a use for them
	// a good use for thefirst one might be to have your name on the screen
	// a good use for the second one might be to have vertex numbers on the screen alongside each vertex

	//glDisable( GL_DEPTH_TEST );
	//glColor3f( 0.f, 1.f, 1.f );
	//DoRasterString( 0.f, 1.f, 0.f, (char *)"Text That Moves" );


	// draw some gratuitous text that is fixed on the screen:
	//
	// the projection matrix is reset to define a scene whose
	// world coordinate system goes from 0-100 in each axis
	//
	// this is called "percent units", and is just a convenience
	//
	// the modelview matrix is reset to identity as we don't
	// want to transform these coordinates
	char levelBuffer[200] = { 0 };
	levelBuffer[50] = '\0';

	char printBuffer[200] = { 0 };
	printBuffer[50] = '\0';

	sprintf(printBuffer, "objColorID: %d", currentObjectPicked);

	glDisable( GL_DEPTH_TEST );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	gluOrtho2D( 0.f, 100.f,     0.f, 100.f );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );
	glColor3f( 1.f, 1.f, 1.f );
	DoRasterString(5.f, 50.f, 0.f, printBuffer);

	sprintf(levelBuffer, "Level %d", currentLevel);
	DoRasterString(2.5f, 95.f, 0.f, levelBuffer);

	//Instructions
	char instrBuffer[1000] = { 0 };
	instrBuffer[999] = '\0';
	if (currentLevel == 0)
	{
		sprintf(instrBuffer, "Click on the Play Button to Begin");
		DoRasterString(5.f, 5.f, 0.f, instrBuffer);
	}
	else if (currentLevel == 1)
	{
		sprintf(instrBuffer, "What's the");
		DoRasterString18(1.f, 12.5f, 0.f, instrBuffer);
		sprintf(instrBuffer, "DIFFERENCE between");
		DoRasterString18(1.f, 7.5f, 0.f, instrBuffer);
		sprintf(instrBuffer, "these two pictures?");
		DoRasterString18(1.f, 2.5f, 0.f, instrBuffer);

	}
	else if (currentLevel == 2)
	{
		DoRasterString10(5.f, 5.f, 0.f, instrBuffer);
	}

	char contrBuffer[1000] = { 0 };
	contrBuffer[999] = '\0';

	if (currentLevel == 1)
	{
		sprintf(contrBuffer, "Controls:");
		DoRasterString(40.f, 10.f, 0.f, contrBuffer);
	}

	// Lock PIN Guess
	char pinBuffer[1000] = { 0 };
	pinBuffer[4] = '\0';
	char keyBuffer[1000] = { 0 };
	keyBuffer[1] = '\0';
	if (currentLevel == 1)
	{
		sprintf(pinBuffer, "%d%d%d%d", lockGuess[0], lockGuess[1], lockGuess[2], lockGuess[3]);
		DoRasterString(81.f, 32.5f, 0.f, pinBuffer);

		sprintf(keyBuffer, "%c", '1');
		DoRasterString(76.5f, 26.f, 0.f, keyBuffer);

		sprintf(keyBuffer, "%c", '2');
		DoRasterString(84.f, 26.f, 0.f, keyBuffer);

		sprintf(keyBuffer, "%c", '3');
		DoRasterString(91.5f, 26.f, 0.f, keyBuffer);

		sprintf(keyBuffer, "%c", '4');
		DoRasterString(76.5f, 18.5f, 0.f, keyBuffer);

		sprintf(keyBuffer, "%c", '5');
		DoRasterString(84.f, 18.5f, 0.f, keyBuffer);

		sprintf(keyBuffer, "%c", '6');
		DoRasterString(91.5f, 18.5f, 0.f, keyBuffer);

		sprintf(keyBuffer, "%c", '7');
		DoRasterString(76.5f, 11.f, 0.f, keyBuffer);

		sprintf(keyBuffer, "%c", '8');
		DoRasterString(84.f, 11.f, 0.f, keyBuffer);

		sprintf(keyBuffer, "%c", '9');
		DoRasterString(91.5f, 11.f, 0.f, keyBuffer);

		sprintf(keyBuffer, "%c", '*');
		DoRasterString(76.5f, 3.5f, 0.f, keyBuffer);

		sprintf(keyBuffer, "%c", '0');
		DoRasterString(84.f, 3.5f, 0.f, keyBuffer);

		sprintf(keyBuffer, "%c", '#');
		DoRasterString(91.5f, 3.5f, 0.f, keyBuffer);
	}
	
	//DoRasterString( 5.f, 5.f, 0.f, (char *)"Text That Doesn't" );

	// swap the double-buffered framebuffers:

	glutSwapBuffers( );

	// be sure the graphics buffer has been sent:
	// note: be sure to use glFlush( ) here, not glFinish( ) !

	glFlush( );
}


void
DoAxesMenu( int id )
{
	AxesOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoColorMenu( int id )
{
	NowColor = id - RED;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDebugMenu( int id )
{
	DebugOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// main menu callback:

void
DoMainMenu( int id )
{
	switch( id )
	{
		case RESET:
			Reset( );
			break;

		case QUIT:
			// gracefully close out the graphics:
			// gracefully close the graphics window:
			// gracefully exit the program:
			glutSetWindow( MainWindow );
			glFinish( );
			glutDestroyWindow( MainWindow );
			exit( 0 );
			break;

		default:
			fprintf( stderr, "Don't know what to do with Main Menu ID %d\n", id );
	}

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoProjectMenu( int id )
{
	NowProjection = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// use glut to display a string of characters using a raster font:

void
DoRasterString( float x, float y, float z, char *s )
{
	glRasterPos3f( (GLfloat)x, (GLfloat)y, (GLfloat)z );

	char c;			// one character to print
	for( ; ( c = *s ) != '\0'; s++ )
	{
		glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, c );
	}
}

void
DoRasterString10(float x, float y, float z, char* s)
{
	glRasterPos3f((GLfloat)x, (GLfloat)y, (GLfloat)z);

	char c;			// one character to print
	for (; (c = *s) != '\0'; s++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, c);
	}
}

void
DoRasterString18(float x, float y, float z, char* s)
{
	glRasterPos3f((GLfloat)x, (GLfloat)y, (GLfloat)z);

	char c;			// one character to print
	for (; (c = *s) != '\0'; s++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}
}

// use glut to display a string of characters using a stroke font:

void
DoStrokeString( float x, float y, float z, float ht, char *s )
{
	glPushMatrix( );
		glTranslatef( (GLfloat)x, (GLfloat)y, (GLfloat)z );
		float sf = ht / ( 119.05f + 33.33f );
		glScalef( (GLfloat)sf, (GLfloat)sf, (GLfloat)sf );
		char c;			// one character to print
		for( ; ( c = *s ) != '\0'; s++ )
		{
			glutStrokeCharacter( GLUT_STROKE_ROMAN, c );
		}
	glPopMatrix( );
}


// return the number of seconds since the start of the program:

float
ElapsedSeconds( )
{
	// get # of milliseconds since the start of the program:

	int ms = glutGet( GLUT_ELAPSED_TIME );

	// convert it to seconds:

	return (float)ms / 1000.f;
}


// initialize the glui window:

void
InitMenus( )
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting InitMenus.\n");

	glutSetWindow( MainWindow );

	int numColors = sizeof( Colors ) / ( 3*sizeof(float) );
	int colormenu = glutCreateMenu( DoColorMenu );
	for( int i = 0; i < numColors; i++ )
	{
		glutAddMenuEntry( ColorNames[i], i );
	}

	int axesmenu = glutCreateMenu( DoAxesMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int debugmenu = glutCreateMenu( DoDebugMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int projmenu = glutCreateMenu( DoProjectMenu );
	glutAddMenuEntry( "Orthographic",  ORTHO );
	glutAddMenuEntry( "Perspective",   PERSP );

	int mainmenu = glutCreateMenu( DoMainMenu );
	glutAddSubMenu(   "Axes",          axesmenu);
	glutAddSubMenu(   "Axis Colors",   colormenu);

	glutAddSubMenu(   "Projection",    projmenu );
	glutAddMenuEntry( "Reset",         RESET );
	glutAddSubMenu(   "Debug",         debugmenu);
	glutAddMenuEntry( "Quit",          QUIT );

// attach the pop-up menu to the right mouse button:

	glutAttachMenu( GLUT_RIGHT_BUTTON );
}



// initialize the glut and OpenGL libraries:
//	also setup callback functions

void
InitGraphics( )
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting InitGraphics.\n");

	// request the display modes:
	// ask for red-green-blue-alpha color, double-buffering, and z-buffering:

	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );

	// set the initial window configuration:

	glutInitWindowPosition( 0, 0 );
	glutInitWindowSize( INIT_WINDOW_SIZE, INIT_WINDOW_SIZE );

	// open the window and set its title:

	MainWindow = glutCreateWindow( WINDOWTITLE );
	glutSetWindowTitle( WINDOWTITLE );

	// set the framebuffer clear values:

	glClearColor( BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3] );

	// setup the callback functions:
	// DisplayFunc -- redraw the window
	// ReshapeFunc -- handle the user resizing the window
	// KeyboardFunc -- handle a keyboard input
	// MouseFunc -- handle the mouse button going down or up
	// MotionFunc -- handle the mouse moving with a button down
	// PassiveMotionFunc -- handle the mouse moving with a button up
	// VisibilityFunc -- handle a change in window visibility
	// EntryFunc	-- handle the cursor entering or leaving the window
	// SpecialFunc -- handle special keys on the keyboard
	// SpaceballMotionFunc -- handle spaceball translation
	// SpaceballRotateFunc -- handle spaceball rotation
	// SpaceballButtonFunc -- handle spaceball button hits
	// ButtonBoxFunc -- handle button box hits
	// DialsFunc -- handle dial rotations
	// TabletMotionFunc -- handle digitizing tablet motion
	// TabletButtonFunc -- handle digitizing tablet button hits
	// MenuStateFunc -- declare when a pop-up menu is in use
	// TimerFunc -- trigger something to happen a certain time from now
	// IdleFunc -- what to do when nothing else is going on

	glutSetWindow( MainWindow );
	glutDisplayFunc( Display );
	glutReshapeFunc( Resize );
	glutKeyboardFunc( Keyboard );
	glutMouseFunc( MouseButton );
	glutMotionFunc( MouseMotion );
	glutPassiveMotionFunc(MouseMotion);
	//glutPassiveMotionFunc( NULL );
	glutVisibilityFunc( Visibility );
	glutEntryFunc( NULL );
	glutSpecialFunc( NULL );
	glutSpaceballMotionFunc( NULL );
	glutSpaceballRotateFunc( NULL );
	glutSpaceballButtonFunc( NULL );
	glutButtonBoxFunc( NULL );
	glutDialsFunc( NULL );
	glutTabletMotionFunc( NULL );
	glutTabletButtonFunc( NULL );
	glutMenuStateFunc( NULL );
	glutTimerFunc( -1, NULL, 0 );

	// setup glut to call Animate( ) every time it has
	// 	nothing it needs to respond to (which is most of the time)
	// we don't need to do this for this program, and really should set the argument to NULL
	// but, this sets us up nicely for doing animation

	glutIdleFunc( Animate );

	// init the glew package (a window must be open to do this):

#ifdef WIN32
	GLenum err = glewInit( );
	if( err != GLEW_OK )
	{
		fprintf( stderr, "glewInit Error\n" );
	}
	else
		fprintf( stderr, "GLEW initialized OK\n" );
	fprintf( stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

	// all other setups go here, such as GLSLProgram and KeyTime setups:

	Pattern.Init( );
	bool valid = Pattern.Create( (char *)"pattern.vert", (char *)"pattern.frag" );
	if( !valid )
		fprintf( stderr, "Could not create the Pattern shader!\n" );
	else
		fprintf( stderr, "Pattern shader created!\n" );

	// set the uniform variables that will not change:
	
	Pattern.Use( );
	Pattern.SetUniformVariable( (char *)"uKa", 0.1f );
	Pattern.SetUniformVariable( (char *)"uKd", 0.5f );
	Pattern.SetUniformVariable( (char *)"uKs", 0.4f );
	Pattern.SetUniformVariable( (char *)"uColor", 1.f, 0.5f, 0.f, 1.f );
	Pattern.SetUniformVariable( (char *)"uSpecularColor", 1.f, 1.f, 1.f, 1.f );
	Pattern.SetUniformVariable( (char *)"uShininess", 12.f );
	Pattern.UnUse( );

	// Pattern Picking
	PatternPicking.Init();
	bool validPicking = PatternPicking.Create((char*)"patternPicking.vert", (char*)"patternPicking.frag" );
	if (!validPicking)
		fprintf(stderr, "Could not create the PatternPicking shader!\n");
	else
		fprintf(stderr, "PatternPicking shader created!\n");

	// Image Difference
	ImageDifference.Init();
	bool validDifference = ImageDifference.Create((char*)"imageDifference.vert", (char*)"imageDifference.frag");
	if (!validDifference)
		fprintf(stderr, "Could not create the ImageDifference shader!\n");
	else
		fprintf(stderr, "ImageDifference shader created!\n");

	ImageDifference.Use();
	//glGenTextures(1, &imageSquareList);
	//glBindTexture(GL_TEXTURE_2D, imageSquareList);
	//ImageDifference.SetUniformVariable((char*)"t", 1.0f);
	ImageDifference.SetUniformVariable((char*)"uTexUnitB", 5);
	ImageDifference.SetUniformVariable((char*)"uTexUnitA", 6);
	//ImageDifference.SetUniformVariable((char*)"uKa", 0.1f);
	//ImageDifference.SetUniformVariable((char*)"uKd", 0.5f);
	//ImageDifference.SetUniformVariable((char*)"uKs", 0.4f);
	//ImageDifference.SetUniformVariable((char*)"uColor", 1.f, 0.5f, 0.f, 1.f);
	//ImageDifference.SetUniformVariable((char*)"uSpecularColor", 1.f, 1.f, 1.f, 1.f);
	//ImageDifference.SetUniformVariable((char*)"uShininess", 12.f);
	ImageDifference.UnUse();

	// Image Blur
	ImageBlur.Init();
	bool validBlur = ImageBlur.Create((char*)"imageBlur.vert", (char*)"imageBlur.frag");
	if (!validBlur)
		fprintf(stderr, "Could not create the ImageBlur shader!\n");
	else
		fprintf(stderr, "ImageBlur shader created!\n");

	ImageBlur.Use();
	ImageBlur.SetUniformVariable((char*)"uImageUnit", 5);
	ImageBlur.UnUse();

	// Image Brightness
	ImageBrightness.Init();
	bool validBrightness = ImageBrightness.Create((char*)"imageBrightness.vert", (char*)"imageBrightness.frag");
	if (!validBrightness)
		fprintf(stderr, "Could not create the ImageBrightness shader!\n");
	else
		fprintf(stderr, "ImageBrightness shader created!\n");

	ImageBrightness.Use();
	ImageBrightness.SetUniformVariable((char*)"uImageUnit", 5);
	ImageBrightness.UnUse();
}


// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )

void
InitLists( )
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting InitLists.\n");

	glutSetWindow( MainWindow );
	/////////////////////
	float BOXSIZE = 2.f;
	float dx = BOXSIZE / 2.f;
	float dy = BOXSIZE / 2.f;
	float dz = BOXSIZE / 2.f;

	// create the object:

	BoxList = glGenLists(1);
	glNewList(BoxList, GL_COMPILE);

	glBegin(GL_QUADS);

	glColor3f(1., 0., 0.);

	glNormal3f(1., 0., 0.);
	glVertex3f(dx, -dy, dz);
	glVertex3f(dx, -dy, -dz);
	glVertex3f(dx, dy, -dz);
	glVertex3f(dx, dy, dz);

	glNormal3f(-1., 0., 0.);
	glVertex3f(-dx, -dy, dz);
	glVertex3f(-dx, dy, dz);
	glVertex3f(-dx, dy, -dz);
	glVertex3f(-dx, -dy, -dz);

	glColor3f(0., 1., 0.);

	glNormal3f(0., 1., 0.);
	glVertex3f(-dx, dy, dz);
	glVertex3f(dx, dy, dz);
	glVertex3f(dx, dy, -dz);
	glVertex3f(-dx, dy, -dz);

	glNormal3f(0., -1., 0.);
	glVertex3f(-dx, -dy, dz);
	glVertex3f(-dx, -dy, -dz);
	glVertex3f(dx, -dy, -dz);
	glVertex3f(dx, -dy, dz);

	glColor3f(0., 0., 1.);

	glNormal3f(0., 0., 1.);
	glVertex3f(-dx, -dy, dz);
	glVertex3f(dx, -dy, dz);
	glVertex3f(dx, dy, dz);
	glVertex3f(-dx, dy, dz);

	glNormal3f(0., 0., -1.);
	glVertex3f(-dx, -dy, -dz);
	glVertex3f(-dx, dy, -dz);
	glVertex3f(dx, dy, -dz);
	glVertex3f(dx, -dy, -dz);

	glEnd();

	glEndList();

	/////////////////////
	// Instructions 1
	/////////////////////
	BOXSIZE = 2.f;
	dx = BOXSIZE / 2.5f;
	dy = BOXSIZE / 5.f;
	dz = BOXSIZE / 2.f;

	// create the object:

	instrList1 = glGenLists(1);
	glNewList(instrList1, GL_COMPILE);

	glBegin(GL_QUADS);

	glColor3f(1., 0., 0.);

	glNormal3f(1., 0., 0.);
	glVertex3f(dx, -dy, dz);
	glVertex3f(dx, -dy, -dz);
	glVertex3f(dx, dy, -dz);
	glVertex3f(dx, dy, dz);

	glNormal3f(-1., 0., 0.);
	glVertex3f(-dx, -dy, dz);
	glVertex3f(-dx, dy, dz);
	glVertex3f(-dx, dy, -dz);
	glVertex3f(-dx, -dy, -dz);

	glColor3f(0., 1., 0.);

	glNormal3f(0., 1., 0.);
	glVertex3f(-dx, dy, dz);
	glVertex3f(dx, dy, dz);
	glVertex3f(dx, dy, -dz);
	glVertex3f(-dx, dy, -dz);

	glNormal3f(0., -1., 0.);
	glVertex3f(-dx, -dy, dz);
	glVertex3f(-dx, -dy, -dz);
	glVertex3f(dx, -dy, -dz);
	glVertex3f(dx, -dy, dz);

	glColor3f(0., 0., 1.);

	glNormal3f(0., 0., 1.);
	glVertex3f(-dx, -dy, dz);
	glVertex3f(dx, -dy, dz);
	glVertex3f(dx, dy, dz);
	glVertex3f(-dx, dy, dz);

	glNormal3f(0., 0., -1.);
	glVertex3f(-dx, -dy, -dz);
	glVertex3f(-dx, dy, -dz);
	glVertex3f(dx, dy, -dz);
	glVertex3f(dx, -dy, -dz);

	glEnd();


	glEndList();

	/////////////////////
	// Controls 1
	/////////////////////
	BOXSIZE = 2.f;
	dx = BOXSIZE / 2.5f;
	dy = BOXSIZE / 5.f;
	dz = BOXSIZE / 2.f;

	// create the object:

	contrList1 = glGenLists(1);
	glNewList(contrList1, GL_COMPILE);

	glBegin(GL_QUADS);

	glColor3f(1., 0., 0.);

	glNormal3f(1., 0., 0.);
	glVertex3f(dx, -dy, dz);
	glVertex3f(dx, -dy, -dz);
	glVertex3f(dx, dy, -dz);
	glVertex3f(dx, dy, dz);

	glNormal3f(-1., 0., 0.);
	glVertex3f(-dx, -dy, dz);
	glVertex3f(-dx, dy, dz);
	glVertex3f(-dx, dy, -dz);
	glVertex3f(-dx, -dy, -dz);

	glColor3f(0., 1., 0.);

	glNormal3f(0., 1., 0.);
	glVertex3f(-dx, dy, dz);
	glVertex3f(dx, dy, dz);
	glVertex3f(dx, dy, -dz);
	glVertex3f(-dx, dy, -dz);

	glNormal3f(0., -1., 0.);
	glVertex3f(-dx, -dy, dz);
	glVertex3f(-dx, -dy, -dz);
	glVertex3f(dx, -dy, -dz);
	glVertex3f(dx, -dy, dz);

	glColor3f(0., 0., 1.);

	glNormal3f(0., 0., 1.);
	glVertex3f(-dx, -dy, dz);
	glVertex3f(dx, -dy, dz);
	glVertex3f(dx, dy, dz);
	glVertex3f(-dx, dy, dz);

	glNormal3f(0., 0., -1.);
	glVertex3f(-dx, -dy, -dz);
	glVertex3f(-dx, dy, -dz);
	glVertex3f(dx, dy, -dz);
	glVertex3f(dx, -dy, -dz);

	glEnd();


	glEndList();

	/////////////////////
	// Keypad 1
	/////////////////////
	BOXSIZE = 2.f;
	dx = BOXSIZE / 16.f;
	dy = BOXSIZE / 16.f;
	dz = BOXSIZE / 16.f;

	// create the object:

	keypad1 = glGenLists(1);
	glNewList(keypad1, GL_COMPILE);

	glBegin(GL_QUADS);

	glColor3f(1., 0., 0.);

	glNormal3f(1., 0., 0.);
	glVertex3f(dx, -dy, dz);
	glVertex3f(dx, -dy, -dz);
	glVertex3f(dx, dy, -dz);
	glVertex3f(dx, dy, dz);

	glNormal3f(-1., 0., 0.);
	glVertex3f(-dx, -dy, dz);
	glVertex3f(-dx, dy, dz);
	glVertex3f(-dx, dy, -dz);
	glVertex3f(-dx, -dy, -dz);

	glColor3f(0., 1., 0.);

	glNormal3f(0., 1., 0.);
	glVertex3f(-dx, dy, dz);
	glVertex3f(dx, dy, dz);
	glVertex3f(dx, dy, -dz);
	glVertex3f(-dx, dy, -dz);

	glNormal3f(0., -1., 0.);
	glVertex3f(-dx, -dy, dz);
	glVertex3f(-dx, -dy, -dz);
	glVertex3f(dx, -dy, -dz);
	glVertex3f(dx, -dy, dz);

	glColor3f(0., 0., 1.);

	glNormal3f(0., 0., 1.);
	glVertex3f(-dx, -dy, dz);
	glVertex3f(dx, -dy, dz);
	glVertex3f(dx, dy, dz);
	glVertex3f(-dx, dy, dz);

	glNormal3f(0., 0., -1.);
	glVertex3f(-dx, -dy, -dz);
	glVertex3f(-dx, dy, -dz);
	glVertex3f(dx, dy, -dz);
	glVertex3f(dx, -dy, -dz);

	glEnd();


	glEndList();

	/////////////////////
	// Image Square List
	/////////////////////
	BOXSIZE = 2.f;
	dx = BOXSIZE / 2.5f;
	dy = BOXSIZE / 2.5f;
	dz = BOXSIZE / 2.5f;

	// create the object:

	imageSquareList = glGenLists(1);
	glNewList(imageSquareList, GL_COMPILE);

	glBegin(GL_QUADS);

	glColor3f(1., 0., 0.);

	glNormal3f(1., 0., 0.);
	glVertex3f(dx, -dy, dz);
	glVertex3f(dx, -dy, -dz);
	glVertex3f(dx, dy, -dz);
	glVertex3f(dx, dy, dz);

	glNormal3f(-1., 0., 0.);
	glVertex3f(-dx, -dy, dz);
	glVertex3f(-dx, dy, dz);
	glVertex3f(-dx, dy, -dz);
	glVertex3f(-dx, -dy, -dz);

	glColor3f(0., 1., 0.);

	glNormal3f(0., 1., 0.);
	glVertex3f(-dx, dy, dz);
	glVertex3f(dx, dy, dz);
	glVertex3f(dx, dy, -dz);
	glVertex3f(-dx, dy, -dz);

	glNormal3f(0., -1., 0.);
	glVertex3f(-dx, -dy, dz);
	glVertex3f(-dx, -dy, -dz);
	glVertex3f(dx, -dy, -dz);
	glVertex3f(dx, -dy, dz);

	glColor3f(0., 0., 1.);

	glNormal3f(0., 0., 1.);
	glVertex3f(-dx, -dy, dz);
	glVertex3f(dx, -dy, dz);
	glVertex3f(dx, dy, dz);
	glVertex3f(-dx, dy, dz);

	glNormal3f(0., 0., -1.);
	glVertex3f(-dx, -dy, -dz);
	glVertex3f(-dx, dy, -dz);
	glVertex3f(dx, dy, -dz);
	glVertex3f(dx, -dy, -dz);

	glEnd();


	glEndList();

	/////////////////////
	// create the object:

	SphereList = glGenLists( 1 );
	glNewList( SphereList, GL_COMPILE );
		OsuSphere( .1, 64, 64 );
	glEndList( );


	// create the axes:

	AxesList = glGenLists( 1 );
	glNewList( AxesList, GL_COMPILE );
		glLineWidth( AXES_WIDTH );
			Axes( 1.5 );
		glLineWidth( 1. );
	glEndList( );
}


// the keyboard callback:

void
Keyboard( unsigned char c, int x, int y )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Keyboard: '%c' (0x%0x)\n", c, c );

	switch( c )
	{
	case 'f':
	case 'F':
		Freeze = !Freeze;
		if (Freeze)
			glutIdleFunc(NULL);
		else
			glutIdleFunc(Animate);
		break;

		case 'o':
		case 'O':
			NowProjection = ORTHO;
			break;

		case 'p':
		case 'P':
			NowProjection = PERSP;
			break;

		case 'q':
		case 'Q':
		case ESCAPE:
			DoMainMenu( QUIT );	// will not return here
			break;				// happy compiler

		default:
			fprintf( stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c );
	}

	// force a call to Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// called when the mouse button transitions down or up:

void
MouseButton( int button, int state, int x, int y )
{
	int b = 0;			// LEFT, MIDDLE, or RIGHT

	if( DebugOn != 0 )
		fprintf( stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y );

	
	// get the proper button bit mask:

	switch( button )
	{
		case GLUT_LEFT_BUTTON:
			b = LEFT;		break;

		case GLUT_MIDDLE_BUTTON:
			b = MIDDLE;		break;

		case GLUT_RIGHT_BUTTON:
			b = RIGHT;		break;

		case SCROLL_WHEEL_UP:
			Scale += SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
			// keep object from turning inside-out or disappearing:
			if (Scale < MINSCALE)
				Scale = MINSCALE;
			break;

		case SCROLL_WHEEL_DOWN:
			Scale -= SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
			// keep object from turning inside-out or disappearing:
			if (Scale < MINSCALE)
				Scale = MINSCALE;
			break;

		default:
			b = 0;
			fprintf( stderr, "Unknown mouse button: %d\n", button );
	}

	// button down sets the bit, up clears the bit:

	if( state == GLUT_DOWN )
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
		// mouse button up
		if (b == LEFT)
		{
			// run picking shader to find what object was selected
			pickingMode = 1;
		}
	}

	glutSetWindow(MainWindow);
	glutPostRedisplay();

}


// called when the mouse moves while a button is down:

void
MouseMotion( int x, int y )
{
	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;

	if( ( ActiveButton & LEFT ) != 0 )
	{
		Xrot += ( ANGFACT*dy );
		Yrot += ( ANGFACT*dx );
	}

	if( ( ActiveButton & MIDDLE ) != 0 )
	{
		Scale += SCLFACT * (float) ( dx - dy );

		// keep object from turning inside-out or disappearing:

		if( Scale < MINSCALE )
			Scale = MINSCALE;
	}

	Xmouse = x;			// new current position
	Ymouse = y;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

void
Reset( )
{
	ActiveButton = 0;
	AxesOn = 1;
	DebugOn = 0;
	Freeze = false;
	Scale  = 1.0;
	NowColor = YELLOW;
	NowProjection = PERSP;
	Xrot = Yrot = 0.;
}


// called when user resizes the window:

void
Resize( int width, int height )
{
	// don't really need to do anything since window size is
	// checked each time in Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// handle a change to the window's visibility:

void
Visibility ( int state )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Visibility: %d\n", state );

	if( state == GLUT_VISIBLE )
	{
		glutSetWindow( MainWindow );
		glutPostRedisplay( );
	}
	else
	{
		// could optimize by keeping track of the fact
		// that the window is not visible and avoid
		// animating or redrawing it ...
	}
}



///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////


// the stroke characters 'X' 'Y' 'Z' :

static float xx[ ] = { 0.f, 1.f, 0.f, 1.f };

static float xy[ ] = { -.5f, .5f, .5f, -.5f };

static int xorder[ ] = { 1, 2, -3, 4 };

static float yx[ ] = { 0.f, 0.f, -.5f, .5f };

static float yy[ ] = { 0.f, .6f, 1.f, 1.f };

static int yorder[ ] = { 1, 2, 3, -2, 4 };

static float zx[ ] = { 1.f, 0.f, 1.f, 0.f, .25f, .75f };

static float zy[ ] = { .5f, .5f, -.5f, -.5f, 0.f, 0.f };

static int zorder[ ] = { 1, 2, 3, 4, -5, 6 };

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void
Axes( float length )
{
	glBegin( GL_LINE_STRIP );
		glVertex3f( length, 0., 0. );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., length, 0. );
	glEnd( );
	glBegin( GL_LINE_STRIP );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., 0., length );
	glEnd( );

	float fact = LENFRAC * length;
	float base = BASEFRAC * length;

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 4; i++ )
		{
			int j = xorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( base + fact*xx[j], fact*xy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 5; i++ )
		{
			int j = yorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( fact*yx[j], base + fact*yy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 6; i++ )
		{
			int j = zorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( 0.0, fact*zy[j], base + fact*zx[j] );
		}
	glEnd( );

}


// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//		glColor3fv( rgb );

void
HsvRgb( float hsv[3], float rgb[3] )
{
	// guarantee valid input:

	float h = hsv[0] / 60.f;
	while( h >= 6. )	h -= 6.;
	while( h <  0. ) 	h += 6.;

	float s = hsv[1];
	if( s < 0. )
		s = 0.;
	if( s > 1. )
		s = 1.;

	float v = hsv[2];
	if( v < 0. )
		v = 0.;
	if( v > 1. )
		v = 1.;

	// if sat==0, then is a gray:

	if( s == 0.0 )
	{
		rgb[0] = rgb[1] = rgb[2] = v;
		return;
	}

	// get an rgb from the hue itself:
	
	float i = (float)floor( h );
	float f = h - i;
	float p = v * ( 1.f - s );
	float q = v * ( 1.f - s*f );
	float t = v * ( 1.f - ( s * (1.f-f) ) );

	float r=0., g=0., b=0.;			// red, green, blue
	switch( (int) i )
	{
		case 0:
			r = v;	g = t;	b = p;
			break;
	
		case 1:
			r = q;	g = v;	b = p;
			break;
	
		case 2:
			r = p;	g = v;	b = t;
			break;
	
		case 3:
			r = p;	g = q;	b = v;
			break;
	
		case 4:
			r = t;	g = p;	b = v;
			break;
	
		case 5:
			r = v;	g = p;	b = q;
			break;
	}


	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}

void
Cross(float v1[3], float v2[3], float vout[3])
{
	float tmp[3];
	tmp[0] = v1[1] * v2[2] - v2[1] * v1[2];
	tmp[1] = v2[0] * v1[2] - v1[0] * v2[2];
	tmp[2] = v1[0] * v2[1] - v2[0] * v1[1];
	vout[0] = tmp[0];
	vout[1] = tmp[1];
	vout[2] = tmp[2];
}

float
Dot(float v1[3], float v2[3])
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}


float
Unit(float vin[3], float vout[3])
{
	float dist = vin[0] * vin[0] + vin[1] * vin[1] + vin[2] * vin[2];
	if (dist > 0.0)
	{
		dist = sqrtf(dist);
		vout[0] = vin[0] / dist;
		vout[1] = vin[1] / dist;
		vout[2] = vin[2] / dist;
	}
	else
	{
		vout[0] = vin[0];
		vout[1] = vin[1];
		vout[2] = vin[2];
	}
	return dist;
}


float
Unit( float v[3] )
{
	float dist = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	if (dist > 0.0)
	{
		dist = sqrtf(dist);
		v[0] /= dist;
		v[1] /= dist;
		v[2] /= dist;
	}
	return dist;
}
