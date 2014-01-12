/****************************************
*     ***************************       *
*         Diplomatiki Ergasia:         *
*                                       *
*       Meleti kai Ylopoiish        *
*       Algorithmon Grafikon        *
*                                       *
*     ***************************       *
*                                       *
*          Syggrafeas:               *
*                                       *
*       Apostolou Panagiotis        *
*                                       *
*     ***************************       *
****************************************/

#ifndef _GLSL_H_
#define _GLSL_H_

#include "../globals.h"
#define MAX_SHADERS 2

//#ifdef JNI_COMPATIBLE
//#   include <jni.h>
//#   include <android/log.h>
//
//#   include <GLES2/gl2.h>
//#   include <GLES2/gl2ext.h>
//#else
//#   include <GL/glew.h>
//#   include <iostream>
//#endif

#include <GL/glew.h>
#include <iostream>
#include <vector>
#include <string.h>

#define VERTEX_ATTRIBUTE_VARIABLE_NAME_VERTICES    "a_vertices"
#define VERTEX_ATTRIBUTE_VARIABLE_NAME_NORMALS     "a_normals"
#define VERTEX_ATTRIBUTE_VARIABLE_NAME_TEXCOORDS   "a_texCoords"
#define VERTEX_ATTRIBUTE_VARIABLE_NAME_COLORS      "a_colors"

#define UNIFORM_VARIABLE_NAME_MODELVIEW_MATRIX     "u_modelviewMatrix"
#define UNIFORM_VARIABLE_NAME_PROJECTION_MATRIX    "u_projectionMatrix"

using namespace std;
class C_GLShader;
typedef enum {NO_SHADER, VERTEX_SHADER, FRAGMENT_SHADER} shader_type_t;

class C_GLShaderObject
{
friend class C_GLShader;
friend class C_GLShaderManager;

public:
   C_GLShaderObject();
   virtual ~C_GLShaderObject();

   bool LoadShaderProgram(const char *);

   //Compile shader
   bool compile(bool printSource);
   char *compilerLog;

protected:
   // shader object
   GLuint shaderObject;
   shader_type_t type;
   GLubyte *shaderSource;
   size_t sourceBytes;
   bool isCompiled;
};

class C_GLVertexShader : public C_GLShaderObject
{
public:
   C_GLVertexShader(void);
   ~C_GLVertexShader(void);
};

class C_GLFragmentShader : public C_GLShaderObject
{
public:
   C_GLFragmentShader(void);
   ~C_GLFragmentShader(void);
};

class C_GLShader
{
friend class C_GLShaderManager;

public:
   C_GLShader(void);
   ~C_GLShader(void);

   void Begin(void);
   void End(void);

   // Uniform Variables
   bool setUniform1f(const char* varname, GLfloat v0);  //!< set float uniform to program
   bool setUniform2f(const char* varname, GLfloat v0, GLfloat v1); //!< set vec2 uniform to program
   bool setUniform3f(const char* varname, GLfloat v0, GLfloat v1, GLfloat v2); //!< set vec3 uniform to program
   bool setUniform4f(const char* varname, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3); //!< set vec4 uniform to program

   bool setUniform1i(const char* varname, GLint v0);
   bool setUniform2i(const char* varname, GLint v0, GLint v1);
   bool setUniform3i(const char* varname, GLint v0, GLint v1, GLint v2);
   bool setUniform4i(const char* varname, GLint v0, GLint v1, GLint v2, GLint v3);

   bool setUniform1fv(const char* varname, GLsizei count, GLfloat *value);
   bool setUniform2fv(const char* varname, GLsizei count, GLfloat *value);
   bool setUniform3fv(const char* varname, GLsizei count, GLfloat *value);
   bool setUniform4fv(const char* varname, GLsizei count, GLfloat *value);
   bool setUniform1iv(const char* varname, GLsizei count, GLint *value);
   bool setUniform2iv(const char* varname, GLsizei count, GLint *value);
   bool setUniform3iv(const char* varname, GLsizei count, GLint *value);
   bool setUniform4iv(const char* varname, GLsizei count, GLint *value);

   bool setUniformMatrix2fv(const char* varname, GLsizei count, GLboolean transpose, GLfloat *value);
   bool setUniformMatrix3fv(const char* varname, GLsizei count, GLboolean transpose, GLfloat *value);
   bool setUniformMatrix4fv(const char* varname, GLsizei count, GLboolean transpose, GLfloat *value);

   GLenum bindAttribLocation(const char *varname, const GLint position);
   GLint getAttribLocation(const char *varname);

   /// Receive Uniform variables:
   void GetUniformfv(char* name, GLfloat* values);
   void GetUniformiv(char* name, GLint* values);

   void printAttribInfo(GLint attrib);

   GLint verticesAttribLocation, normalsAttribLocation, texCoordsAttribLocation, colorsAttribLocation;

protected:
   char *linkerLog;

   // Add a vertex or fragment shader
   void AddShader(C_GLShaderObject* shader);
   // Link shaders
   bool Link(void);
   inline bool GetisLinked(void) { return isLinked; }
   void UpdateAttribLocations(void);

private:
   // Holds all the shaders
   C_GLShaderObject* shaderList[MAX_SHADERS];
   int nShaders;
   GLuint programObject;
   bool isLinked;
   bool inUse;

   GLint GetUniLoc(const GLcharARB *name);
};

class C_GLShaderManager
{
public :
   ~C_GLShaderManager();
   static C_GLShaderManager *getSingleton(void);

   // Load a vertex and a fragment shader and returns a pointer to a C_GLShader object that holds both
   C_GLShader* LoadShaderProgram(const char *, const char *);

private:
   C_GLShaderManager();

   static bool instanceFlag;
   static C_GLShaderManager *classInstance;

   static vector<C_GLShader *> shaderList;
   static C_GLShaderObject *shaderObjectExists(const C_GLShaderObject *shaderObject, shader_type_t type);
   static C_GLShader *shaderExists(const C_GLShaderObject *shaderObject1, const C_GLShaderObject *shaderObject2);
};

//Initializes extensions using glew
//#ifndef JNI_COMPATIBLE
bool InitGLExtensions(void);
//#endif

//Checks if glsl can be used
bool CheckGLSL(void);

#endif
