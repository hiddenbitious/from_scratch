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
#define VERTEX_ATTRIBUTE_VARIABLE_NAME_TANGENTS    "a_tangents"
#define VERTEX_ATTRIBUTE_VARIABLE_NAME_BINORMALS   "a_binormals"

#define UNIFORM_VARIABLE_NAME_MODELVIEW_MATRIX     "u_modelviewMatrix"
#define UNIFORM_VARIABLE_NAME_PROJECTION_MATRIX    "u_projectionMatrix"
#define UNIFORM_VARIABLE_NAME_MVP_MATRIX           "u_mvpMatrix"
#define UNIFORM_VARIABLE_NAME_MODEL_MATRIX         "u_modelMatrix"
#define UNIFORM_VARIABLE_NAME_VIEW_MATRIX          "u_viewMatrix"

#define UNIFORM_VARIABLE_NAME_TEXTURE_0            "u_texture_0"
#define UNIFORM_VARIABLE_NAME_TEXTURE_1            "u_texture_1"
#define UNIFORM_VARIABLE_NAME_TEXTURE_2            "u_texture_2"

#define UNIFORM_VARIABLE_NAME_TEXTURE_DIFFUSE      "u_texture_diffuse"
#define UNIFORM_VARIABLE_NAME_TEXTURE_NORMAL_MAP   "u_texture_normal_map"
#define UNIFORM_VARIABLE_NAME_TEXTURE_SPECULAR     "u_texture_specular"

#define UNIFORM_VARIABLE_LIGHT_POSITION            "u_lightPosition_es"

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

   bool LoadShaderProgram(const char *filename);
   bool compile(bool printSource);
   char *compilerLog;

protected:
   GLuint         shaderObject;  /// This is the shader ID returned from GL
   shader_type_t  type;
   GLubyte        *shaderSource;
   size_t         sourceBytes;
   bool           isCompiled;
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

   inline GLint GetUniLoc(const GLcharARB *name) { return glGetUniformLocation(programObject, name); }

   /// Uniform Variables
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
   GLint tangetsAttribLocation, binormalsAttribLocation;
   GLint textureUniformLocation_0, textureUniformLocation_1, textureUniformLocation_2;
   GLint textureDiffuseLocation_0, textureNormalMapLocation_1, textureSpecularLocation_2;

protected:
   void              AddShader(C_GLShaderObject* shader);      /// Add a vertex or fragment shader
   bool              Link(void);                               /// Link shaders
   void              UpdateAttribLocations(void);
   inline bool       GetisLinked(void) { return isLinked; }

   char              *linkerLog;

private:
   C_GLShaderObject  *shaderList[MAX_SHADERS];                 /// Holds all the shaders
   int               nShaders;
   GLuint            programObject;                            /// Shader ID returned from glCreatePrograms
   bool              isLinked;
   bool              inUse;
};

class C_GLShaderManager
{
public :
   ~C_GLShaderManager();
   static C_GLShaderManager   *getSingleton(void);

   /// Load a vertex and a fragment shader and returns a pointer to a C_GLShader object that holds both
   C_GLShader                 *LoadShaderProgram(const char *, const char *);
   void                       pushShader(C_GLShader *shader);
   void                       popShader(void);

private:
   C_GLShaderManager();

   static bool                instanceFlag;
   static C_GLShaderManager   *classInstance;

   vector<C_GLShader *>       shaderList;       /// All loaded shaders
   vector<C_GLShader *>       activeShader;     /// Stack of active shaders
   bool                       markForDeactivation;

   /// Finds if a shader has already been loaded by comparing it's source
   C_GLShaderObject           *shaderObjectExists(const C_GLShaderObject *shaderObject, shader_type_t type);
   C_GLShader                 *shaderExists(const C_GLShaderObject *shaderObject1, const C_GLShaderObject *shaderObject2);
};

/// Initializes extensions using glew
//#ifndef JNI_COMPATIBLE
bool InitGLExtensions(void);
//#endif

//Checks if glsl can be used
bool CheckGLSL(void);

#endif
