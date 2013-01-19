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

#pragma comment( lib, "..\\debug\\oglRenderer.lib" )

#include <GL\GLEW.h>
#include <GL\GLUT.h>

#include "camera.h"
#include "vectors.h"
#include "quaternion.h"
#include "meshgroup.h"
#include "3DS reader\3dsreader.h"
#include "timer.h"
#include "bspTree.h"

#include "..\oglRenderer\oglDriver.h"
#include "..\oglRenderer\shaders\glsl.h"

using namespace std;

// Diloseis sinartiseon
void SetVSync ( bool status );
void SetRelevantDir ( LPWSTR dir );
void CountFPS ( void );
LPWSTR szAppPath = (LPWSTR)new wchar_t[256];

// BSP
C_BspTree bspTest ( 15 );
bool drawNarn = false;
bool wireframe = false;

// Shaders
C_GLShaderManager shaderManager;
C_GLShader* basicShader;
C_GLShader* basicShader_texture;

// Antikeimena
C_MeshGroup bunny;
C_MeshGroup xwing;

//Camera kai frustum
C_Camera camera;
C_Frustum frustum;

// Metablites gia to parathiro
int winID;
int windowWidth = 500;
int windowHeight = 500;
int windowPositionX = 200;
int windowPositionY = 200;

// Tahitita metakinisis
float speed = 7.0f;

// Metablites hronometrisis
C_Timer timer;
ULONG start = timer.GetTime ();
float timeElapsed = 0.0f;
float fps;


void CountFPS ( void )
{
	static ULONG count = 0.0f;
	ULONG delta = timer.GetTime () - start;
	count++;

	if ( delta >= 1000 )
	{
		fps = count;
		start = timer.GetTime ();
		count = 0;
	}
}


// Set up the rendering state
void SetupRC ( void )
{
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

//	glEnable ( GL_CULL_FACE );
	glFrontFace ( GL_CW );
//	glCullFace ( GL_BACK );

	glEnable ( GL_DEPTH_TEST );
	glShadeModel ( GL_SMOOTH );
//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDisable ( GL_NORMALIZE );

	glDisable ( GL_TEXTURE_GEN_S );
	glDisable ( GL_TEXTURE_GEN_T );
	glDisable ( GL_TEXTURE_GEN_R );
	glDisable ( GL_TEXTURE_GEN_Q );

	glEnableClientState ( GL_VERTEX_ARRAY );
	glEnableClientState ( GL_NORMAL_ARRAY );
	glEnableClientState ( GL_TEXTURE_COORD_ARRAY );

//	glDisable ( GL_LIGHTING );
	glEnable ( GL_LIGHTING );

	float ambient[] = { 1.0f , 1.0f , 1.0f , 1.0f };
	float diffuse[] = { 1.0f , 1.0f , 1.0f , 1.0f };
	float specular[] = { 1.0f , 1.0f , 1.0f , 1.0f };
//	float lightPos[] = { 0.0f , 0.0f , -1.0f , 0.0f };		//Directional light
	glLightfv ( GL_LIGHT0 , GL_AMBIENT , ambient );
	glLightfv ( GL_LIGHT0 , GL_DIFFUSE , diffuse );
	glLightfv ( GL_LIGHT0 , GL_SPECULAR , specular );
//	glLightfv ( GL_LIGHT0 , GL_POSITION , lightPos );
	glEnable ( GL_LIGHT0 );


	camera.frustum = &frustum;
	camera.fov = 50.0f;
	camera.zFar = 1800.0f;
	camera.zNear = 1.0f;


	// Load shaders
	SetRelevantDir ( L"shaders" );
	basicShader = shaderManager.LoadShaderProgram ( "basic.vert" , "basic.frag" );



	// Load raw geometry from file and build a BSP tree
	SetRelevantDir ( L"" );
	if ( !bspTest.ReadGeometryFile ( "properMap2.bsp" )  )
		exit ( -1 );

	bspTest.BuildBspTree ();
	bspTest.BuildPVS ();

	float sizes[2];
	glGetFloatv ( GL_POINT_SIZE_RANGE , sizes );
	glPointSize ( (sizes[1]-sizes[0]) / 5.0f );


	C_Vector3 scalef = C_Vector3 ( 50.0f , 50.0f , 50.0f );

	C_3DSReader reader;
	//SetRelevantDir ( L"models\\narn" );
	SetRelevantDir ( L"models" );
	reader.Load3DSFile ( "statue.3ds" , &group );// , false, &scalef );
	reader.Load3DSFile ( "statue.3ds" , &group2 );//, false , &scalef );
	group2.Translate ( 0.0f , 0.0f , 40.0f );
	group.Rotate ( 0.0f , -45.0f , 0.0f );
	group2.Rotate ( 0.0f , 45.0f , 0.0f );

	glTexEnvi ( GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE );
	glEnable(GL_TEXTURE_2D);

	timer.Initialize ();
}


void Draw ( void )
{
	static float angle = 0.5f;
	float mat[16];

	
	C_Quaternion quat;
	quat.EulerToQuat ( angle , angle , 0.0 );
	quat.QuaternionToMatrix16 ( mat );
	angle += 1.0f * timeElapsed;
	if ( angle >= 360.0f ) angle = 0.0f;


	// Clear the window with current clearing color
//	glClearColor ( 0 , 0 ,0 ,0 );
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	objectsInFrustum = 0;

	camera.Look ();
//	glTranslatef ( 0.0 , 0.0 , -150.0 );
	//Draw a sphere in the center of the world
//	glutSolidSphere ( 4.0 , 20 , 20 );


	float lightPos[] = { camera.position.x + 10.0f*sin(angle) , camera.position.y , camera.position.z + 10.0f*cos(angle) , 1.0f };
	float zero[] = { 0.0f , 0.0f , 0.0f , 1.0f };
	glPushMatrix ();
		glTranslatef ( lightPos[0],lightPos[1],lightPos[2] );
		glutSolidSphere ( 1.5 , 10 , 10 );
	glPopMatrix ();
	glLightfv ( GL_LIGHT0 , GL_POSITION , lightPos );



	if ( wireframe == true )
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode ( GL_FRONT , GL_FILL );

	objectsInFrustum += group2.Draw ( &frustum );
	group2.DrawBVolumes ();

	basicShader->Begin ();


		objectsInFrustum += group.Draw ( &frustum );
		group.DrawBVolumes ();

		switch ( wut )
		{
		case 1:
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			bspTest.Draw_PVS ( &(camera.GetPosition () ) );
		break;

		case 2:
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			//bspTest.Draw ();
			bspTest.Draw2 ( &(camera.GetPosition () ) );
		break;

		case 3:
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			bspTest.Draw_PVS ( &(camera.GetPosition () ) );
		break;
		}
	basicShader->End ();

	timer.Update ();
	timeElapsed = timer.GetDelta () / 1000.0f;

	CountFPS ();

	_DrawFrameBuffer ();
}


void reshape ( GLint w , GLint h )
{
	windowWidth = w;
	windowHeight = h;

	camera.setProjection ( w , h );
}


void idle ( void )
{
	//Update window name
	ostringstream stnObjects , stFps , mapPolys;
	stnObjects << objectsInFrustum;
	stFps << fps;
	mapPolys << mapPolygonsDrawn;
	string str = "Objects in Frustum: " + stnObjects.str () + " fps: " + stFps.str () + " map polys: " + mapPolys.str ();
	glutSetWindowTitle( str.c_str () );

	glutPostRedisplay ();
}


void mouse_look ( int x , int y)
{
	static int oldX = 0;
	static int oldY = 0;
	float xx = 360 * (windowWidth - x)/windowWidth;
	float yy = 360 * (windowHeight - y)/windowHeight;

	if ( oldY != yy )
	{
		camera.Rotate ( 2*(yy-oldY) , 0.0 );
		oldY = yy;
	}
	if ( oldX != xx )
	{
		camera.Rotate ( 0.0 , 2*(xx-oldX) );
		oldX = xx;
	}

	glutPostRedisplay();
}


void hande_simple_keys (unsigned char key , int x , int y)
{
	switch( key )
	{
		case 27 : case 13 :	//ESC
		break;

		case 'f' :
			bspTest.IncreaseNodesDrawn ();
		break;

		case 'v' :
			bspTest.DecreaseNodesDrawn ();
		break;

		case 'a' : case 'A' :
			//camera.MoveUp ( speed );
			wut = 2;
		break;

		case 'z' : case 'Z' :
			//camera.MoveDown ( speed );
			wut = 3;
		break;

		case 's' : case 'S' :
			drawNarn = true;
		break;

		case 'x' : case 'X' :
			drawNarn = false;
		break;

		case 'd' : case 'D' :
			bspTest.IncreaseLeavesDrawn ();
		break;

		case 'c' : case 'C' :
			bspTest.DecreaseLeavesDrawn ();
		break;

		default:
			cout << int ( key ) << '\n';
		break;
	}	
}


void handle_arrows( int key , int x , int y )
{
	switch( key )
	{
		case GLUT_KEY_UP:
			camera.Move( speed );
		break;

		case GLUT_KEY_DOWN:
			camera.Move ( -speed );
		break;

		case GLUT_KEY_RIGHT:
			camera.StrafeRight ( speed );
		break;

		case GLUT_KEY_LEFT:
			camera.StrafeLeft ( speed );
		break;
	}

	glutPostRedisplay ();
}


// Main program entry point
int main(int argc, char* argv[])
{
	glutInit ( &argc , argv );
	glutInitDisplayMode ( GL_DOUBLE | GLUT_RGB | GLUT_DEPTH );

	glutInitWindowSize ( windowWidth , windowHeight );
	glutInitWindowPosition ( windowPositionX , windowPositionY );

	winID = glutCreateWindow ( "..." );

	GetCurrentDirectory ( 256 , szAppPath );

	glutReshapeFunc ( reshape );
	glutDisplayFunc ( Draw );
	glutPassiveMotionFunc ( mouse_look );
	glutSpecialFunc( handle_arrows );
	glutKeyboardFunc( hande_simple_keys );
	glutIdleFunc ( idle );


	// Init glew
	InitGLExtensions ();
	// Check if we can use glsl
	CheckGLSL ();

	// Switch VSYNC off
	SetVSync ( false );

	SetupRC();

	glutMainLoop();
	return 0;
}


void SetVSync ( bool status )
{
	const char *szExt = (char *)glGetString(GL_EXTENSIONS);

	if((szExt != NULL) && (strstr(szExt, "WGL_EXT_swap_control") != NULL))
	{
		typedef bool (APIENTRY *PFNWGLSWAPINTERVALFARPROC) (int);
		PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = NULL;

		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress( "wglSwapIntervalEXT" );

		if ( status )
			cout << "VSYNC in now ON.\n" << endl;
		else
			cout << "VSYNC is now OFF.\n" << endl;

		if(wglSwapIntervalEXT != NULL) wglSwapIntervalEXT ( status );
	}
	else
	{
		cout << "Extension \"WGL_EXT_swap_control\" is not supported." << endl;
		cout << "Cannot set vsync status.\n" << endl;
	}
}


void SetRelevantDir ( LPWSTR dir )
{
	LPWSTR szTempPath = (LPWSTR)new wchar_t[256];
	wcscpy_s ( szTempPath , 256 , szAppPath );

	if ( dir[0] != '\\' ) wcscat_s ( szTempPath , 256 , L"\\" );
	wcscat_s ( szTempPath , 256 , dir );

	SetCurrentDirectory ( szTempPath );
	delete[] szTempPath;
}
