#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#define DRAW_BSP_GEOMETRY              false
#define DRAW_TREE_MESHES               true
#define ENABLE_MESH_FRUSTUM_CULLING    true
#define ENABLE_BSP_FRUSTUM_CULLING     true
#define USE_PVS                        true

#define ENABLE_COLLISION_DETECTION     true
#define USE_HIGH_QUALITY_SHADERS       true

#define PRINT_TREE_STATISTICS          false

#define ROTATE_MESH_BBOXES             true

//#define JNI_COMPATIBLE

#ifdef JNI_COMPATIBLE
#	include <jni.h>
#	include <android/log.h>

#	define  LOGI(...)	__android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#	define  LOGE(...)	__android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#else
#	define  LOGI(...) 	fprintf(stdout, __VA_ARGS__)
#	define  LOGE(...)		fprintf(stderr, __VA_ARGS__)

//#define PRINT_FUNC_ENTRY   printf("%s\n", __FUNCTION__)
#define PRINT_FUNC_ENTRY

#	include <stdint.h>
#	include <string>
#	include <limits>
#	include <GL/glew.h>

#  ifdef DISABLE_ASSERTS
#     define assert(cond)
#  else
#	   include <assert.h>
#  endif // DISABLE_ASSERTS

using namespace std;

#endif

/// -------------------------
/// Basic structs and types

typedef unsigned long   ULONG;
typedef unsigned int    UINT;
typedef unsigned short  USHORT;

typedef struct {
	float x , y , z;
} C_Vertex;

typedef struct {
	float nx , ny , nz;
} C_Normal;

typedef struct {
	float u , v;
} C_TexCoord;

typedef struct {
	float r , g , b , a;
} C_Color;

typedef struct {
	UINT p0 , p1 , p2;
} C_TriIndices;

typedef struct {
    GLfloat   m[4][4];
} ESMatrix;

struct triangle_vn {
	C_Vertex vertex0;
	C_Vertex normal0;
	C_Vertex vertex1;
	C_Vertex normal1;
	C_Vertex vertex2;
	C_Vertex normal2;
};

struct triangle_vnt {
	C_Vertex vertex0;
	C_Vertex normal0;
	C_TexCoord  tex0;
	C_Vertex vertex1;
	C_Vertex normal1;
	C_TexCoord  tex1;
	C_Vertex vertex2;
	C_Vertex normal2;
	C_TexCoord  tex2;
};

typedef enum {
	txUnknown	= 0,	// images
	txBmp		= 1,
	txJpg		= 2,
	txPng		= 3,
	txTga		= 4,
	txGif		= 5,
	txIco		= 6,
	txEmf		= 7,
	txWmf		= 8
} eglTexType;

typedef enum {
	txNoFilter	= 0,
	txBilinear	= 1,
	txTrilinear	= 2
} eglTexFilterType;

typedef enum {
   X_MINUS,
   Z_MINUS,
   X_PLUS,
   Z_PLUS,

   Y_MINUS,
   Y_PLUS,

   TOTAL_FACES
} world_direction_t;

typedef	struct {
	uint32_t 	TextureID;     // Texture ID Used To Select A Texture
	eglTexType	TexType;       // Texture Format
	uint32_t	Width;            // Image Width
	uint32_t	Height;           // Image Height
	uint32_t	Type;             // Image Type (GL_RGB, GL_RGBA)
	uint32_t	Bpp;              // Image Color Depth In Bits Per Pixel
	std::string	textureName;
} glTexture;

/// -------------------------
/// Forward declarations

class C_3DSReader;
class C_Camera;
class C_Quaternion;
class vector2;
class C_Vector3;
class C_GLShaderManager;
class C_TextureManager;
class C_GLShader;
class C_InputHandler;
class C_Mob;
class C_Mesh;
class C_MeshGroup;

/// -------------------------
/// Global variables

extern ESMatrix            globalViewMatrix, globalProjectionMatrix, globalMVPMatrix;
extern C_GLShaderManager   *shaderManager;
extern C_TextureManager    *textureManager;
extern C_GLShader          *bspShader;
extern C_GLShader          *basicShader;
extern C_GLShader          *pointShader;
extern C_GLShader          *wallShader;
extern C_GLShader          *simple_texture_shader;
extern C_Vertex            lightPosition;
extern char                MAX_THREADS;
extern C_Camera            camera;
extern C_InputHandler      inputHandler;
extern C_Mob mob;


extern    C_MeshGroup wallMesh;
extern    C_MeshGroup wallMesh2;
extern    C_MeshGroup floorMesh;
extern    C_MeshGroup floorMesh2;
extern    C_MeshGroup floorMesh3;
extern    C_MeshGroup floorMesh4;
extern    C_MeshGroup grating;
extern    C_MeshGroup corner_inner;
extern    C_MeshGroup corner_outer;


#endif // _GLOBALS_H_
