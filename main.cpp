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

#include <GL/glew.h>
#include <GL/glut.h>

#include "camera.h"
#include "vectors.h"
#include "quaternion.h"
#include "meshgroup.h"
#include "3dsReader/3dsReader.h"
//#include "timer.h"
#include "bspTree.h"

#include "../oglRenderer/oglDriver.h"
#include "../oglRenderer/shaders/glsl.h"

#include "metaballs/cubeGrid.h"
#include "metaballs/metaball.h"


using namespace std;

// Diloseis sinartiseon
//void SetRelevantDir ( LPWSTR dir );
//void CountFPS ( void );
//LPWSTR szAppPath = (LPWSTR)new wchar_t[256];

// BSP
// Orise ena dendro me megisto bathos 15
static C_BspTree bspTest ( 15 );

// Shaders. Tha hrisimopoiithoun 2 shaders
static C_GLShaderManager shaderManager;
static C_GLShader* basicShader;
static C_GLShader* basicShader_texture;

// Dinamika 3D antikeimena
static C_MeshGroup bunny;
static C_MeshGroup narn;
static C_MeshGroup statue1;
static C_MeshGroup statue2;

// Camera kai frustum
static C_Camera camera;
static C_Frustum frustum;

// Metablites gia to parathiro
static int winID;
static int windowWidth = 800;
static int windowHeight = 500;
static int windowPositionX = 100;
static int windowPositionY = 200;

// Tahitita metakinisis cameras
static float speed = 7.0f;
static float angle = 0.5f;
static float angle2 = 0.5f;

static int meshesInFrustum = 0;
static int meshesPolys = 0;
static int metaballPolys = 0;
static int mapPolys = 0;
static bool frustumCulling = true;
static int bspRenderingType = 0;

// Hrisimes statheres
static float red[] = { 1.0f , 0.0f , 0.0f , 1.0f };
static float white[] = { 1.0f , 1.0f , 1.0f , 1.0f };
static float grey[] = { 0.3f , 0.3f , 0.3f , 0.3f };
static C_Vector3 center ( 0.0f , 0.0f , 0.0f );

// Metablites hronometrisis
//C_Timer timer;
//ULONG start = timer.GetTime ();
static float timeElapsed = 0.0f;
static float fps;

// Metaballs
static C_CubeGrid grid;
static C_Metaball metaball[3];



// Sinartisi arhikpoiiseon
static void Initializations ( void )
{
	// Hroma background
	glClearColor ( 0.3671875f , 0.15234375f , 0.8359375f , 1.0f );

	// Backface culling
	glEnable ( GL_CULL_FACE );
	glFrontFace ( GL_CW );
	glCullFace ( GL_BACK );

	// Energopoiise depth test
	glEnable ( GL_DEPTH_TEST );
	// Gouraud shading
	glShadeModel ( GL_SMOOTH );

	// Apenergopoiise diafora states
	glDisable ( GL_NORMALIZE );

	// Energopoiise vertex arrays
	glEnableClientState ( GL_VERTEX_ARRAY );
	glEnableClientState ( GL_NORMAL_ARRAY );
	glEnableClientState ( GL_TEXTURE_COORD_ARRAY );

	// Energopoiise fotismo
	glEnable ( GL_LIGHTING );

	// Parametroi gia to fos tis skinis
	float ambient[] = { 1.0f , 1.0f , 1.0f , 1.0f };
	float diffuse[] = { 1.0f , 1.0f , 1.0f , 1.0f };
	float specular[] = { 1.0f , 1.0f , 1.0f , 1.0f };
	glLightfv ( GL_LIGHT0 , GL_AMBIENT , ambient );
	glLightfv ( GL_LIGHT0 , GL_DIFFUSE , diffuse );
	glLightfv ( GL_LIGHT0 , GL_SPECULAR , specular );

	// Energopoiise ena fos
	glEnable ( GL_LIGHT0 );

	// Enose tin camera me to frustum kai dose times gia tin proboli
	camera.frustum = &frustum;
	camera.fov = 50.0f;
	camera.zFar = 1800.0f;
	camera.zNear = 1.0f;

	// Fortose shaders
//	SetRelevantDir ( L"shaders" );
	basicShader = shaderManager.LoadShaderProgram ( "basic.vert" , "basic.frag" );
	basicShader_texture = shaderManager.LoadShaderProgram ( "basic_withSingleTexture.vert" , "basic_withSingleTexture.frag" );

	// Diabase tin geometria gia to bsp
//	SetRelevantDir ( L"" );
	bspTest.ReadGeometryFile ( "properMap2.bsp" );

	// Kataskeuase bsp kai pvs
	bspTest.BuildBspTree ();
	bspTest.BuildPVS ();

	// Fortose dinamika andikeimena
	C_3DSReader reader;


//	SetRelevantDir ( L"models" );
	reader.Load3DSFile ( "statue.3ds" , &statue1 );
	reader.Load3DSFile ( "statue.3ds" , &statue2 );
	statue2.Translate ( 0.0f , 0.0f , 40.0f );
	statue1.Rotate ( 0.0f , -45.0f , 0.0f );

	C_Vector3 scale = C_Vector3 ( 150.0f , 150.0f , 150.0f );
	reader.Load3DSFile ( "bunny.3ds" , &bunny , true , &scale );
	bunny.SetPosition ( 80.0f , 0.0f , 0.0f );

//	SetRelevantDir ( L"models//narn" );
	scale = C_Vector3 ( 0.1f , 0.1f , 0.1f );
	reader.Load3DSFile ( "narn.3ds" , &narn , &scale );
	narn.SetPosition ( -100.0f , 10.0f , 0.0f );
	narn.Rotate ( 90.0f , 0.0f , 0.0f );


	// Energopoiise textures
	glTexEnvi ( GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE );
	glEnable ( GL_TEXTURE_2D );


	// Arhikopoiiseis Metaballs
	grid.Constructor ( -250.0f , 0.0f , 200.0f );

	metaball[0].Constructor ();
	metaball[0].position.x = 10.0f;
	metaball[0].position.y = 10.0f;
	metaball[0].position.z = 10.0f;
	metaball[0].radius = 5.0f;


	metaball[1].Constructor ();
	metaball[1].position.x = 10.0f;
	metaball[1].position.y = 10.0f;
	metaball[1].position.z = 10.0f;
	metaball[1].radius = 8.0f;

	metaball[2].Constructor ();
	metaball[2].position.x = 15.0f;
	metaball[2].position.y = 15.0f;
	metaball[2].position.z = 15.0f;
	metaball[2].radius = 3.0f;

	// Arhikopoiise ton timer
//	timer.Initialize ();
}


static void Draw ( void )
{
	C_Vector3 cameraPosition = camera.GetPosition();

	// Statheri tahitita peristrofis, aneksartita ta posa fps ehoume.
	angle += 1.0f * timeElapsed;
	if ( angle >= 360.0f ) angle = 0.0f;

	angle2 += 2.0f * timeElapsed;
	if ( angle2 >= 360.0f ) angle2 = 0.0f;


	// These to hroma tou parathirou sto default kai katharise ton depth buffer
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	camera.Look ();
	meshesInFrustum = 0;
	meshesPolys = 0;
	metaballPolys = 0;

	// To fos akolouthei tin camera kai peristrefete giro apo autin
	float lightPos[] = { cameraPosition.x + 10.0f * sin(angle) ,
						 cameraPosition.y ,
						 cameraPosition.z + 10.0f * cos(angle) , 1.0f };
	float zero[] = { 0.0f , 0.0f , 0.0f , 1.0f };

	// Shediase mia mikri sfaira sti thesi tou fotos
	glPushMatrix ();
		glTranslatef ( lightPos[0],lightPos[1],lightPos[2] );
		glutSolidSphere ( 1.5 , 10 , 10 );
	glPopMatrix ();

	// Topothetise to fos
	glLightfv ( GL_LIGHT0 , GL_POSITION , lightPos );


	// Shediase ti geometria pou den hrisimopoiei textures
	basicShader->Begin ();

	// Shediase mia sfaira sto kendro ton aksonon
		glColor3fv ( red );
		glutSolidSphere ( 4.0 , 20 , 20 );

		glColor3fv ( white );
		if ( frustumCulling )
		{
			meshesInFrustum += bunny.Draw ( camera.frustum , &meshesPolys );
			bunny.DrawBVolumes ();
		}
		else
			bunny.Draw ( );

		bunny.Rotate ( 0.0f , timeElapsed * 15.0f , 0.0f );


		// Shediase to dendro. To dendro den ehei textures.

		glColor3fv ( white );
		// Se periptosi pou den ehoume shaders
		if ( !basicShader->GetisLinked () || !CheckGLSL () )
		{
			glMaterialfv ( GL_FRONT , GL_DIFFUSE , white );
			glMaterialfv ( GL_FRONT , GL_AMBIENT , grey );
		}

		switch ( bspRenderingType ) {
			// Shediase ti fainetai kanonika.
			case 0:
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				mapPolys = bspTest.Draw_PVS (&cameraPosition);
			break;

			// Shediase olo to harti se wireframe
			case 1:
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				mapPolys = bspTest.Draw2 (&cameraPosition);
			break;

			// Shediase to PVS se wireframe
			case 2:
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				mapPolys = bspTest.Draw_PVS (&cameraPosition);
			break;
		}


		// Shediase tis metaballs
		metaball[0].position.y = 20.0f + 5 * cos ( angle2 );
		metaball[0].position.x = 20.0f + 10 * cos ( angle2 );

		metaball[1].position.x = 20.0f + 8.0f * cos ( angle );
		metaball[1].position.z = 20.0f + 5.0f * cos ( angle );

		metaball[2].position.z = 15.0f + 10.0f * cos ( angle );
		if ( frustumCulling )
		{
			grid.Update ( metaball , 3 , camera.frustum );
			metaballPolys = grid.Draw ( camera.frustum );
			grid.DrawGridCube ();
		}
		else
		{
			grid.Update ( metaball , 3 , NULL );
			grid.Draw ( NULL );
		}

	basicShader->End ();


	// Shediase ta dinamika andikeimena pou hrisimopoioun textures
	basicShader_texture->Begin ();
	glColor3fv ( white );
		if ( frustumCulling )
		{
			meshesInFrustum  += statue1.Draw ( camera.frustum , &meshesPolys );
			statue1.DrawBVolumes ();
		}
		else
			statue1.Draw ( );


		// Periestrepse to deutero agalma giro apo to kentro ton aksonon
		statue2.Rotate ( 0.0f , timeElapsed * 15.0f , 0.0f , &center );
		if ( frustumCulling )
		{
			meshesInFrustum  += statue2.Draw ( camera.frustum , &meshesPolys );
			statue2.DrawBVolumes ();
		}
		else
			statue2.Draw ( );


	// Shediase to diastimoploio. Kanto na horopidaei.
		narn.Rotate ( 0.0f , timeElapsed * 15.0f , 0.0f );
		narn.SetPosition ( -100.0f , 10.0f * cos ( angle ) , 0.0f );
		if ( frustumCulling )
		{
			meshesInFrustum  += narn.Draw ( camera.frustum , &meshesPolys );
			narn.DrawBVolumes ();
		}
		else
			narn.Draw ( );

	basicShader_texture->End ();





	// Tipose dedomena stin othoni
	int line = 1;
	int lineHeight = 18;
	camera.PrintText ( 0 , lineHeight * line++ ,
					   1.0f , 1.0f , 0.0f , 0.6f ,
					   "FPS: %d" , (int)fps );
	camera.PrintText ( 0 , lineHeight * line++ ,
					   1.0f , 1.0f , 0.0f , 0.6f ,
					   "Mesa sto frustum briskondai %d andikeimena pou apoteloundai apo %d trigona" , meshesInFrustum , meshesPolys );
	camera.PrintText ( 0 , lineHeight * line++ ,
					   1.0f , 1.0f , 0.0f , 0.6f ,
					   "Oi metaballs apoteloundai apo %d trigona" , metaballPolys );
	camera.PrintText ( 0 , lineHeight * line++ ,
					   1.0f , 1.0f , 0.0f , 0.6f ,
					   "To dendro zografise %d trigona" , mapPolys );
	camera.PrintText ( 0 , lineHeight * line++ ,
					   1.0f , 1.0f , 0.0f , 0.6f ,
					   "Sinolo %d trigona" , mapPolys + metaballPolys + meshesPolys );

	line++;
	camera.PrintText ( 0 , lineHeight * line++ ,
					   1.0f , 1.0f , 0.0f , 0.6f ,
					   "a,z: Camera epano/kato" );
	camera.PrintText ( 0 , lineHeight * line++ ,
					   1.0f , 1.0f , 0.0f , 0.6f ,
					   "s: Energopoiisi/Apenergopoiisi frustum culling" );
	camera.PrintText ( 0 , lineHeight * line++ ,
					   1.0f , 1.0f , 0.0f , 0.6f ,
					   "x: Allagi methodou shediasis BSP" );

	// Ananeose to hronometro.
//	timer.Update ();
	// H timeElapsed krataei to hrono pou hreiastike gia na sxediastei 1 frame.
//	timeElapsed = timer.GetDelta () / 1000.0f;

	// Metrise ta fps
//	CountFPS ();

	// Zografise ton frame buffer!
	_DrawFrameBuffer ();
}

// Sinartisi heirismou parathirou
static void reshape ( GLint w , GLint h )
{
	windowWidth = w;
	windowHeight = h;

	// Se periptosi pou allakse to parathiro prepei na ananeothei i proboli
	camera.setProjection ( w , h );
}


static void idle ( void )
{
	glutPostRedisplay ();
}


// Sinartisi heirismou tou mouse
static void mouse_look ( int x , int y)
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

	// An kounithike to mouse prepei na ksanasxediasoume ti skini
	glutPostRedisplay();
}


// Sinartisi heirismou keyboard.
static void hande_simple_keys (unsigned char key , int x , int y)
{
	switch( key )
	{
		case 27 : case 13 :	//ESC
			exit ( 0 );
		break;

		case 'f' :
			bspTest.IncreaseNodesDrawn ();
		break;

		case 'v' :
			bspTest.DecreaseNodesDrawn ();
		break;

		case 'a' : case 'A' :
			camera.MoveUp ( speed );
		break;

		case 'z' : case 'Z' :
			camera.MoveDown ( speed );
		break;

		case 's' : case 'S' :
			frustumCulling = !frustumCulling;
		break;

		case 'x' : case 'X' :
			bspRenderingType++;
			if ( bspRenderingType > 2 )
				bspRenderingType = 0;
		break;

		default:
			cout << int ( key ) << '\n';
		break;
	}
}


// Sinartisi heirismou arrow keys.
static void handle_arrows( int key , int x , int y )
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

	// Ta arrow keys eleghoun tin kamera ara prepei na ksanasxediasoume ti skini
	glutPostRedisplay ();
}


// Main program entry point
int main(int argc, char* argv[])
{
	// Arhikopoiisi tou glut
	glutInit ( &argc , argv );

	// Double buffer me depth buffer
	glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

	// Kataskeui window
	glutInitWindowSize ( windowWidth , windowHeight );
	glutInitWindowPosition ( windowPositionX , windowPositionY );

	winID = glutCreateWindow ( "..." );

	// Arhikopoiisi tou directory.
//	GetCurrentDirectory ( 256 , szAppPath );

	glutReshapeFunc ( reshape );
	glutDisplayFunc ( Draw );
	glutPassiveMotionFunc ( mouse_look );
	glutSpecialFunc( handle_arrows );
	glutKeyboardFunc( hande_simple_keys );
	glutIdleFunc ( idle );


	// Arhikopoiisi tou glew
	InitGLExtensions ();

	// Eleghei an einai diathesimoi oi shaders (GLSL)
	CheckGLSL ();

	// Apenergopoiei (an einai dinaton) to VSYNC
	_SetVSync ( false );

	// Kalese ti sinartisi arhikopoiisis
	Initializations ();

	// Ksekina to loop
	glutMainLoop();
	return 0;
}



//void SetRelevantDir ( LPWSTR dir )
//{
//	LPWSTR szTempPath = (LPWSTR)new wchar_t[256];
//	wcscpy_s ( szTempPath , 256 , szAppPath );
//
//	if ( dir[0] != '\\' ) wcscat_s ( szTempPath , 256 , L"\\" );
//	wcscat_s ( szTempPath , 256 , dir );
//
//	SetCurrentDirectory ( szTempPath );
//	delete[] szTempPath;
//}
//
//
//void CountFPS ( void )
//{
//	static ULONG count = 0.0f;
//	ULONG delta = timer.GetTime () - start;
//	count++;
//
//	if ( delta >= 1000 )
//	{
//		fps = count;
//		start = timer.GetTime ();
//		count = 0;
//	}
//}
