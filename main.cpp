/****************************************
*     ***************************       *
*         Diplomatiki Ergasia:			*
*                                       *
*		  Meleti kai Ylopoiish			*
*		  Algorithmon Grafikon			*
*                                       *
*     ***************************       *
*                                       *
*			  Syggrafeas:               *
*                                       *
*		  Apostolou Panagiotis			*
*                                       *
*     ***************************       *
****************************************/

#include <iostream>
#include <sstream>
#include <time.h>

#ifndef JNI_COMPATIBLE
#	include <GL/glew.h>
#	include <GL/glut.h>
#else
#	include <GLES2/gl2.h>
#	include <GLES2/gl2ext.h>

#	include <stdio.h>
#	include <stdlib.h>
#	include <math.h>
#endif

#include "camera.h"
#include "vectors.h"
#include "timer.h"

#include "glsl/glsl.h"

#include "metaballs/cubeGrid.h"
#include "metaballs/metaball.h"

using namespace std;

/// Global variables
ESMatrix globalModelviewMatrix, globalProjectionMatrix;

/// Shaders
//static C_GLShaderManager shaderManager;
//static C_GLShader* basicShader;
//static C_GLShader* metaballShader;
//static C_GLShader* basicShader_texture;

/// Camera and frustum
static C_Camera camera;
static C_Frustum frustum;

/// window stuff
static int winID;
static int windowWidth = 800;
static int windowHeight = 500;
static int windowPositionX = 100;
static int windowPositionY = 200;

/// movement vars
static float speed = 7.0f;
static float angle = 0.5f;
static float angle2 = 0.5f;

static int metaballPolys = 0;
static bool frustumCulling = true;

//static float white[] = { 1.0f , 1.0f , 1.0f , 1.0f };
//static float grey[] = { 0.3f , 0.3f , 0.3f , 0.3f };
static C_Vector3 center(0.0f , 0.0f , 0.0f);

/// Timer vars
C_Timer timer;
float start = timer.GetTime ();
static float timeElapsed = 0.0f;
static float fps;

/// Metaballs
static C_CubeGrid grid;
static C_Metaball metaball[3];

static void CountFPS (void);

// Sinartisi arhikpoiiseon
static void Initializations(void)
{
	/// Set clear color
	glClearColor(0.3671875f , 0.15234375f , 0.8359375f , 1.0f);

	/// Backface culling
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);

	/// XXX: Disable normalizing ???
	glDisable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);

	/// vertex arrays
//	glEnableClientState(GL_VERTEX_ARRAY);
//	glEnableClientState(GL_NORMAL_ARRAY);
//	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	/// Lighting parameters
	float ambient[] = { 1.0f , 1.0f , 1.0f , 1.0f };
	float diffuse[] = { 1.0f , 1.0f , 1.0f , 1.0f };
	float specular[] = { 1.0f , 1.0f , 1.0f , 1.0f };
	glLightfv(GL_LIGHT0 , GL_AMBIENT , ambient);
	glLightfv(GL_LIGHT0 , GL_DIFFUSE , diffuse);
	glLightfv(GL_LIGHT0 , GL_SPECULAR , specular);

	glEnable(GL_LIGHT0);

	// Enose tin camera me to frustum kai dose times gia tin proboli
	camera.frustum = &frustum;
	camera.fov = 50.0f;
	camera.zFar = 800.0f;
	camera.zNear = 1.0f;

	/// Load shaders
//	basicShader_texture = shaderManager.LoadShaderProgram("basic_withSingleTexture.vert" , "basic_withSingleTexture.frag");

	/// metaballs initialization
	grid.Constructor(0.0f , 0.0f , -80.0f);

	metaball[0].Constructor();
	metaball[0].position.x = 10.0f;
	metaball[0].position.y = 10.0f;
	metaball[0].position.z = 10.0f;
	metaball[0].radius = 5.0f;

	metaball[1].Constructor();
	metaball[1].position.x = 10.0f;
	metaball[1].position.y = 10.0f;
	metaball[1].position.z = 10.0f;
	metaball[1].radius = 8.0f;

	metaball[2].Constructor();
	metaball[2].position.x = 15.0f;
	metaball[2].position.y = 15.0f;
	metaball[2].position.z = 15.0f;
	metaball[2].radius = 3.0f;

	/// timer initialization
	timer.Initialize ();
}

static void Draw(void)
{
	C_Vector3 cameraPosition = camera.GetPosition();

	/// Make the angle rotation independant of the cpu speed
	angle += .2f * timeElapsed;
	if(angle >= 360.0f) { angle = 0.0f; }

	angle2 += .05f * timeElapsed;
	if(angle2 >= 360.0f) { angle2 = 0.0f; }

	/// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	esMatrixLoadIdentity(&globalModelviewMatrix);
	camera.Look();
	metaballPolys = 0;

	/// Make light source rotate around the camera
	float lightPos[] = { cameraPosition.x + 10.0f * (float)sin(angle),
						 cameraPosition.y,
						 cameraPosition.z + 10.0f * (float)cos(angle), 1.0f};

	/// Visuallize light by placing a low poly sphere at it's position
//	glPushMatrix();
//	glTranslatef(lightPos[0], lightPos[1], lightPos[2]);
//	glutSolidSphere(1.5, 5, 5);
//	glPopMatrix();

	/// Place light source
//	glLightfv(GL_LIGHT0 , GL_POSITION , lightPos);

//	glColor3fv(white);
	/// In case shaders are not available
//	if(!basicShader->GetisLinked() || !CheckGLSL()) {
//		glMaterialfv(GL_FRONT , GL_DIFFUSE , white);
//		glMaterialfv(GL_FRONT , GL_AMBIENT , grey);
//	}

	/// Draw metaballs
	metaball[0].position.y = 20.0f + 5 * cos(angle2);
	metaball[0].position.x = 20.0f + 10 * cos(angle2);

	metaball[1].position.x = 20.0f + 8.0f * cos(angle);
	metaball[1].position.z = 20.0f + 5.0f * cos(angle);

	metaball[2].position.z = 15.0f + 10.0f * cos(angle);

//	basicShader->Begin();
//	if(frustumCulling) {
//		grid.Update(metaball , 3 , camera.frustum);
//		metaballPolys = grid.Draw(camera.frustum);
////		grid.DrawGridCube();
//	} else {
		grid.Update(metaball , 3 , NULL);
		grid.Draw(NULL);
//	}
//	basicShader->End();

#ifndef JNI_COMPATIBLE
	/// Print text on screem
	int line = 1;
	int lineHeight = 18;
	camera.PrintText(0 , lineHeight * line++ ,
					 1.0f , 1.0f , 0.0f , 0.6f ,
					 "FPS: %d" , (int)fps);
	camera.PrintText(0, lineHeight * line++,
					 1.0f, 1.0f, 0.0f, 0.6f,
					 "Metaball polys: %d" , metaballPolys);

	/// Update timer
	timer.Update ();
	timeElapsed = timer.GetDelta () / 1000.0f;

	CountFPS ();

	glutSwapBuffers();
#endif
}


#ifndef JNI_COMPATIBLE
// Sinartisi heirismou parathirou
static void reshape(GLint w , GLint h)
{
	windowWidth = w;
	windowHeight = h;
	camera.setProjection(w , h);
}

static void idle(void)
{
	glutPostRedisplay();
}

/// mouse handling
static void mouse_look(int x , int y)
{
	static int oldX = 0;
	static int oldY = 0;
	float xx = 360 * (windowWidth - x) / windowWidth;
	float yy = 360 * (windowHeight - y) / windowHeight;

	if(oldY != yy) {
		camera.Rotate(2 * (yy - oldY) , 0.0);
		oldY = yy;
	}

	if(oldX != xx) {
		camera.Rotate(0.0 , 2 * (xx - oldX));
		oldX = xx;
	}

	glutPostRedisplay();
}

/// keyboard handling
static void hande_simple_keys(unsigned char key , int x , int y)
{
	switch(key) {
		case 27 : case 13 :	//ESC
			exit(0);
			break;

		case 'a' : case 'A' :
			camera.MoveUp(speed);
			break;

		case 'z' : case 'Z' :
			camera.MoveDown(speed);
			break;

		case 's' : case 'S' :
			frustumCulling = !frustumCulling;
			break;

		default:
			cout << int (key) << '\n';
			break;
	}
}


/// arrow keys handling
static void handle_arrows(int key , int x , int y)
{
	switch(key) {
		case GLUT_KEY_UP:
			camera.Move(speed);
			break;

		case GLUT_KEY_DOWN:
			camera.Move(-speed);
			break;

		case GLUT_KEY_RIGHT:
			camera.StrafeRight(speed);
			break;

		case GLUT_KEY_LEFT:
			camera.StrafeLeft(speed);
			break;
	}

	glutPostRedisplay();
}
#endif

// Main program entry point
int main(int argc, char* argv[])
{
#ifndef JNI_COMPATIBLE
	glutInit(&argc , argv);

	/// Double buffering with depth buffer
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	/// Create window
	glutInitWindowSize(windowWidth , windowHeight);
	glutInitWindowPosition(windowPositionX , windowPositionY);

	winID = glutCreateWindow("...");

	glutReshapeFunc(reshape);
	glutDisplayFunc(Draw);
	glutPassiveMotionFunc(mouse_look);
	glutSpecialFunc(handle_arrows);
	glutKeyboardFunc(hande_simple_keys);
	glutIdleFunc(idle);

	InitGLExtensions();
	CheckGLSL();
	Initializations();

	glutMainLoop();
#else
	CheckGLSL();
	Initializations();
#endif

	return 0;
}

void CountFPS (void)
{
	static ULONG count = 0.0f;
	float delta = timer.GetTime () - start;
	count++;

	if(delta >= 1000.0f) {
		fps = count;
		start = timer.GetTime ();
		count = 0;
	}
}
