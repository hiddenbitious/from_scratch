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
#include "mesh.h"
#include "map.h"
#include "timer.h"
#include "input.h"
#include "actor.h"
#include "glsl/glsl.h"
#include "metaballs/cubeGrid.h"
#include "metaballs/metaball.h"
#include "sound.h"
#include "textRenderer/textRenderer.h"

#include "battleMap/battleMap.h"
#include "battleMap/battleTile.h"

using namespace std;

int mapPolys;
static C_Map map;
C_Vertex lightPosition;
C_MeshGroup cube;

/// Global variables
ESMatrix globalViewMatrix, globalProjectionMatrix, globalMVPMatrix;

C_GLShaderManager *shaderManager = NULL;
C_TextureManager  *textureManager = NULL;
C_SoundManager    *soundManager = NULL;

char MAX_THREADS = 0;

C_GLShader *bspShader = NULL;
C_GLShader *basicShader = NULL;
C_GLShader *pointShader = NULL;
C_GLShader *wallShader = NULL;
C_GLShader *simple_texture_shader = NULL;

/// Camera and frustum
C_Camera camera;
static C_Frustum frustum;
static C_Party party;
C_Mob mob;
C_InputHandler inputHandler;
C_BattleMap battleMap(&camera);

/// window stuff
static int winID;
static int windowWidth = 800;
static int windowHeight = 600;
static int windowPositionX = 200;
static int windowPositionY = 200;
static int fontSize = 48;

/// movement vars
static float speed = 7.0f;

static C_Vector3 center(0.0f , 0.0f , 0.0f);

/// Timer vars
C_Timer timer;
static float timerStart;
static float timePerFrame = 0.017f;
static int fps = 60.0f;
#define FPS (1000.0f / timePerFrame)

/// Metaballs
static C_CubeGrid *grid;
static C_Metaball metaball[3];
//AUDIO!!!
sf::SoundBuffer bufIntro;
sf::SoundBuffer bufStep;
sf::SoundBuffer bufRush;
sf::SoundBuffer bufTurn;
sf::Sound sndIntro;
sf::Sound sndStep;
sf::Sound sndRush;
sf::Sound sndTurn;

static void CountFPS (void);

static void
Initializations(void)
{
    /// Find number of available cores
    MAX_THREADS = get_nprocs();
    printf("Detected %d cpu cores.\n", MAX_THREADS);

    printf("EPSILON: %g\n", EPSILON);
    printf("GREATEST_FLOAT: %g\n", GREATEST_FLOAT);
    printf("SMALLEST_FLOAT: %g\n", SMALLEST_FLOAT);
    printf("FLT_MAX: %g\n", FLT_MAX);
    printf("FLT_MIN: %g\n", FLT_MIN);

    /// Set clear color
    if(DRAW_BSP_GEOMETRY) {
        glClearColor(0.3671875f , 0.15234375f , 0.8359375f , 1.0f);
    } else {
        glClearColor(0.0, 0.0, 0.0, 1.0f);
    }

    /// Backface culling
    if(DRAW_BSP_GEOMETRY) {
        glDisable(GL_CULL_FACE);
    } else {
        glEnable(GL_CULL_FACE);
    }

    glEnable(GL_DEPTH_TEST);
    glPointSize(10.0f);

    /// XXX: Disable normalizing ???
    glDisable(GL_NORMALIZE);

    // Enose tin camera me to frustum kai dose times gia tin proboli
    camera.frustum = &frustum;
    camera.fov = 70.0f;
    camera.zFar = 1000.0f;
    camera.zNear = 1.0f;

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

    wallShader = shaderManager->LoadShaderProgram("shaders/shader1.vert", "shaders/shader1.frag");
    assert(wallShader->verticesAttribLocation >= 0);

    simple_texture_shader = shaderManager->LoadShaderProgram("shaders/simple_texture.vert", "shaders/simple_texture.frag");
    assert(simple_texture_shader->verticesAttribLocation >= 0);

    /// Texture manager
    textureManager = C_TextureManager::getSingleton();

    /// Sound manager
    soundManager = C_SoundManager::GetSingleton();

    map.createMap("map");

    int tileStartx, tileStarty;
    C_Vertex cameraPosition = map.cameraStartPosition(&tileStartx, &tileStarty);
    printf("****\nx: %d, y: %d\n", tileStartx, tileStarty);
    printf("cameraPosition: %f %f %f\n", cameraPosition.x, cameraPosition.y, cameraPosition.z);
    camera.SetPosition(cameraPosition);
    camera.Rotate(0.0f, 180.0f);

    cube.loadFromFile("objmodels/cube.obj");
    cube.initVBOS();
    cube.shader = simple_texture_shader;

    party.setMap(&map);
    party.setCoordinates(tileStartx + 5, tileStarty);

    mob.setMap(&map);
    mob.loadModel();
    mob.setCoordinates(tileStartx + 5, tileStarty);


    for (int x = 0; x < WORLD_WIDTH / 128; x ++) {
        for (int y = 0; y < WORLD_HEIGHT / 128; y ++) {
            battleMap.AddStaticObject(new C_BattleTile(&battleMap, x * 128.f, 0.f, y * 128.f, 0.f));
        }
    }

    /// timer initialization
    timer.Initialize();
    timer.Update();
    timerStart = timer.GetTime();

    textRenderInit("fonts/FreeSans.ttf", fontSize);
}

static void
shutdown(void)
{
    delete shaderManager;

    map.~C_Map();
}

static void
Draw(void)
{
    soundManager->PlaySound(SOUND_RUSH);

    static float angle = 0.0f;
    static float radius = 4.0f;
    C_Vertex offset;
    /// Clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    camera.Look();

    offset.x = radius * cos(angle);
    offset.z = radius * sin(angle);
    offset.y = 0.0f;

    cube.position.x = camera.position.x + offset.x;
    cube.position.y = camera.position.y;
    cube.position.z = camera.position.z + offset.z;

    lightPosition = math::transformPoint(&globalViewMatrix, &cube.position);

    angle += 1.1f / FPS;
    if(angle >= 360.0f) angle = 0.0f;

    cube.draw(&camera);
    map.draw(&camera);

    party.update(FPS);

    mob.update(FPS);
    mob.Draw(&camera);

    /// Update timer
	timer.Update();
	timePerFrame = timer.GetDelta();
//	printf("tileElapsed: %f ms\n", timePerFrame);
//	printf("estimated fps %f \n:", (1000.0f / timePerFrame));

    CountFPS ();

    glutSwapBuffers();
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
    float xx = 360.0f * (windowWidth - x) / windowWidth;
    float yy = 360.0f * (windowHeight - y) / windowHeight;

    if(oldY != (int)yy) {
        camera.Rotate(2 * (yy - oldY) , 0.0);
        oldY = yy;
    }

    if(oldX != (int)xx) {
        camera.Rotate(0.0 , 2 * (xx - oldX));
        oldX = xx;
    }

    glutPostRedisplay();
}

static void
hande_simple_keys(unsigned char key , int x , int y)
{
//   printf("%c pressed\n", key);

    if(key >= 'A' && key <= 'w') {
        inputHandler.pressKey(key);
    } else {
        switch(key) {
        case 27 :
        case 13 :	//ESC
            shutdown();
            exit(0);
            break;

        case 'z' :
        case 'Z' :
            camera.MoveUp(speed);
            break;

        case 'x' :
        case 'X' :
            camera.MoveDown(speed);
            break;

        default:
            cout << int (key) << '\n';
            break;
        }
    }
}

static void
release_simple_keys(unsigned char key,int x,int y)
{
//   printf("%c released\n", key);

    if(key >= 'A' && key <= 'w')
        inputHandler.releaseKey(key);
}

/// arrow keys handling
static void
handle_arrows(int key , int x , int y)
{
    switch(key) {
    case GLUT_KEY_UP:
//         camera.Move(speed);
        party.move(MOVE_FORWARD);
        break;

    case GLUT_KEY_DOWN:
//         camera.Move(-speed);
        party.move(MOVE_BACKWARDS);
        break;

    case GLUT_KEY_RIGHT:
//         camera.StrafeRight(speed);
        party.move(MOVE_TURN_RIGHT);
        break;

    case GLUT_KEY_LEFT:
//         camera.StrafeLeft(speed);
        party.move(MOVE_TURN_LEFT);
        break;
    }

    glutPostRedisplay();
}
#endif

void
CountFPS (void)
{
    static uint32_t count = 0.0f;
    static float totalTime = 0.0f;
    totalTime += timePerFrame;
    count++;

    static float color[] = {1.0, 1.0, 1.0, 1.0};
    textRenderDrawText("fps " + std::to_string(fps), -1.0f, 1.0f - (30.0f) / windowHeight, 1.0f / (16.0f * fontSize), 1.0f / (16.0f * fontSize), color);

    if(totalTime >= 1000.0f) {
        fps = count;
//        printf("fps: %d\n", fps);
        count = 0;
        totalTime = 0.0f;
    }
}

int
main(int argc, char* argv[])
{
#ifndef JNI_COMPATIBLE
    glutInit(&argc, argv);

    /// Double buffering with depth buffer
    glutInitDisplayMode(/*GLUT_DOUBLE | */GLUT_RGB | GLUT_DEPTH);

    /// Create window
    glutInitWindowSize(windowWidth , windowHeight);
    glutInitWindowPosition(windowPositionX , windowPositionY);

    winID = glutCreateWindow("...");

    glutReshapeFunc(reshape);
    glutDisplayFunc(Draw);
    glutIdleFunc(idle);

//    glutPassiveMotionFunc(mouse_look);

    glutKeyboardUpFunc(release_simple_keys);
    glutKeyboardFunc(hande_simple_keys);
    glutSpecialFunc(handle_arrows);

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
