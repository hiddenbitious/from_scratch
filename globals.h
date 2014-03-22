#ifndef _GLOBALS_H_
#define _GLOBALS_H_

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
#	include <assert.h>
#	include <GL/glew.h>

using namespace std;

#endif

typedef unsigned long ULONG;
typedef unsigned int UINT;
typedef unsigned short USHORT;

//Very simple struct to hold vertex information
typedef struct {
	float x , y , z;
} C_Vertex;

//Very simple struct to hold normal information
typedef struct {
	float nx , ny , nz;
} C_Normal;

//Very simple struct to hold texture coordinate information
typedef struct {
	float u , v;
} C_TexCoord;

//Very simple struct to hold color information
typedef struct {
	float r , g , b , a;
} C_Color;

//Very simple struct to hold index information
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
	txWmf		= 8,
} eglTexType;

typedef enum {
	txNoFilter	= 0,
	txBilinear	= 1,
	txTrilinear	= 2,
} eglTexFilterType;

typedef	struct {
	uint32_t 	TextureID;     // Texture ID Used To Select A Texture
	eglTexType	TexType;       // Texture Format
	uint32_t	Width;            // Image Width
	uint32_t	Height;           // Image Height
	uint32_t	Type;             // Image Type (GL_RGB, GL_RGBA)
	uint32_t	Bpp;              // Image Color Depth In Bits Per Pixel
	std::string	textureName;
} glTexture;

class C_3DSReader;
class C_Camera;
class C_Quaternion;
class vector2;
class C_Vector3;
class C_GLShaderManager;
class C_TextureManager;
class C_GLShader;

extern ESMatrix globalModelviewMatrix, globalProjectionMatrix;
extern ESMatrix globalViewMatrix;
extern C_GLShaderManager *shaderManager;
extern C_TextureManager *textureManager;
extern C_GLShader *bspShader;
extern C_GLShader *basicShader;
extern C_GLShader *pointShader;
extern C_GLShader *wallShader;

extern bool drawConnectedToo;
extern char MAX_THREADS;

#endif
