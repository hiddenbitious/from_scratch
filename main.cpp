#include <iostream>
#include <sstream>
#include <time.h>
#include <stdio.h>
#include <sys/sysinfo.h>

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
#include "bspTree.h"
#include "bspNode.h"
#include "vectors.h"
#include "timer.h"
#include "glsl/glsl.h"
#include "metaballs/cubeGrid.h"
#include "metaballs/metaball.h"

using namespace std;

// BSP
static C_BspTree *bspTest;
int mapPolys;
bool drawConnectedToo = false;

/// Global variables
ESMatrix globalModelviewMatrix, globalProjectionMatrix;
C_GLShaderManager *shaderManager = NULL;
char MAX_THREADS = 0;

C_GLShader *bspShader = NULL;
C_GLShader *basicShader = NULL;
C_GLShader *pointShader = NULL;

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
static int bspRenderingType = 0;

static C_Vector3 center(0.0f , 0.0f , 0.0f);

/// Timer vars
C_Timer timer;
float start = timer.GetTime ();
static float timeElapsed = 0.0f;
static float fps;

/// Metaballs
static C_CubeGrid *grid;
static C_Metaball metaball[3];

static void CountFPS (void);

static void
Initializations(void)
{
	/// Find number of cores available
   MAX_THREADS = get_nprocs();
   printf("Detected %d cpu cores.\n", MAX_THREADS);

	/// Set clear color
	glClearColor(0.3671875f , 0.15234375f , 0.8359375f , 1.0f);

	/// Backface culling
	glDisable(GL_CULL_FACE);
//	glEnable(GL_CULL_FACE);
//	glFrontFace(GL_CCW);
//	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);
	glPointSize(10.0f);
//	glShadeModel(GL_SMOOTH);

	/// XXX: Disable normalizing ???
	glDisable(GL_NORMALIZE);

	// Enose tin camera me to frustum kai dose times gia tin proboli
	camera.frustum = &frustum;
	camera.fov = 50.0f;
	camera.zFar = 1000.0f;
	camera.zNear = 1.0f;

	// Diabase tin geometria gia to bsp
	bspTest = new C_BspTree(15);
//	bspTest->ReadGeometryFile("properMap2.BSP");
	bspTest->ReadMapFile("map.txt");

	// Kataskeuase bsp kai pvs
	bspTest->BuildBspTree();
//	bspTest->BuildPVS();

	/// metaballs initialization
	grid = new C_CubeGrid();
	grid->Constructor(0.0f , 0.0f , -80.0f);

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

	/// Shaders
	shaderManager = C_GLShaderManager::getSingleton();

	bspShader = shaderManager->LoadShaderProgram("shaders/metaballs_shader.vert", "shaders/metaballs_shader.frag");
   assert(bspShader->verticesAttribLocation >= 0);
   assert(bspShader->normalsAttribLocation >= 0);

   basicShader = shaderManager->LoadShaderProgram("shaders/wire_shader.vert", "shaders/wire_shader.frag");
   assert(basicShader->verticesAttribLocation >= 0);
   assert(basicShader->normalsAttribLocation == -1);

   pointShader = shaderManager->LoadShaderProgram("shaders/points_shader.vert", "shaders/points_shader.frag");
   assert(pointShader->verticesAttribLocation >= 0);
   assert(pointShader->normalsAttribLocation == -1);

	/// timer initialization
	timer.Initialize ();
}

static void
Draw(void)
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

	/// Draw metaballs
	metaball[0].position.y = 20.0f + 5 * cos(angle2);
	metaball[0].position.x = 20.0f + 10 * cos(angle2);

	metaball[1].position.x = 20.0f + 8.0f * cos(angle);
	metaball[1].position.z = 20.0f + 5.0f * cos(angle);

	metaball[2].position.z = 15.0f + 10.0f * cos(angle);

   grid->Update(metaball , 3 , NULL);
   grid->Draw(NULL);

//	switch(bspRenderingType) {
//		case 0:
//		   /// Draw tree using PVS
//			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//			mapPolys = bspTest->Draw_PVS(&cameraPosition);
//			break;
//
//      case 1:
         /// Draw the whole tree without PVS
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			mapPolys = bspTest->Draw2(&cameraPosition);
//			break;
//
//		case 1:
//		   /// Draw PVS in wireframe mode
//			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//			mapPolys = bspTest->Draw_PVS(&cameraPosition);
//			break;
//
////      case 3:
////  			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
////         mapPolys = bspTest->Draw2(&cameraPosition);
////         glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
////			bspTest->Draw3();
////			break;
//	}

#ifndef JNI_COMPATIBLE
	/// Print text on screem
	int line = 1;
	int lineHeight = 18;
	camera.PrintText(0 , lineHeight * line++ ,
					 1.0f , 1.0f , 0.0f , 0.6f ,
					 "FPS: %d" , (int)fps);
	camera.PrintText(0, lineHeight * line++,
					 1.0f, 1.0f, 0.0f, 0.6f,
					 "bsp polys: %d" , mapPolys);
   camera.PrintText(0, lineHeight * line++,
					 1.0f, 1.0f, 0.0f, 0.6f,
					 "bsp rendering method: %d", bspRenderingType);

	/// Update timer
	timer.Update ();
	timeElapsed = timer.GetDelta () / 1000.0f;

	CountFPS ();

	glutSwapBuffers();
#endif
}


#ifndef JNI_COMPATIBLE
static void
reshape(GLint w , GLint h)
{
	windowWidth = w;
	windowHeight = h;
	camera.setProjection(w , h);
}

static void
idle(void)
{
	glutPostRedisplay();
}

static void
mouse_look(int x , int y)
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

static void
hande_simple_keys(unsigned char key , int x , int y)
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

		case 'x' : case 'X' :
			bspRenderingType = (bspRenderingType + 1) % 2;
			printf("bspRenderingType: %d\n", bspRenderingType);
			break;

		case 'q' : case 'Q' :{
			bspTest->IncreaseLeavesDrawn();

			printf("Drawing leaf: %d (%lu)\n", bspTest->leafToDraw, bspTest->leaves[bspTest->leafToDraw]->nodeID);
			int nleaves = bspTest->leaves[bspTest->leafToDraw]->connectedLeaves.size();
			printf("Connected leaves: %d\n", nleaves);
			for(int i = 0; i < nleaves; i++) {
   			printf("%lu, ", bspTest->leaves[bspTest->leafToDraw]->connectedLeaves[i]->nodeID);
         }
         printf("\n\n");

			break;
			}

		case 'w' : case 'W' :{
			bspTest->DecreaseLeavesDrawn();

			printf("Drawing leaf: %d (%lu)\n", bspTest->leafToDraw, bspTest->leaves[bspTest->leafToDraw]->nodeID);
			int nleaves = bspTest->leaves[bspTest->leafToDraw]->connectedLeaves.size();
			printf("Connected leaves: %d\n", nleaves);
			for(int i = 0; i < nleaves; i++) {
   			printf("%ld, ", bspTest->leaves[bspTest->leafToDraw]->connectedLeaves[i]->nodeID);
         }
         printf("\n\n");

			break;
			}

      case 'e': {
         drawConnectedToo = !drawConnectedToo;
         break;
      }

		default:
			cout << int (key) << '\n';
			break;
	}
}

/// arrow keys handling
static void
handle_arrows(int key , int x , int y)
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

int
main(int argc, char* argv[])
{
#ifndef JNI_COMPATIBLE
	glutInit(&argc, argv);

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

void
CountFPS (void)
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
