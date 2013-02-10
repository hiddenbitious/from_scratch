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

#ifndef _GLSL_H_
#define _GLSL_H_

#include <GL/glew.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
class C_GLShader;

class C_GLShaderObject {
		friend class C_GLShader;


	public:
		C_GLShaderObject();
		virtual ~C_GLShaderObject();

		//Load shader program from file
		bool LoadShaderProgram(const char* filename);
		//Compile shader
		bool compile(bool printSource);

		// Log returner after compiling the shader
		string compilerLog;

	protected:

		// shader object
		GLuint shaderObject;
		// 1 = Vertex shader, 2 = Fragment shader, 0 = none
		int type;

		// Shader's source code
		GLubyte* shaderSource;

		// Set to true if compilation was successful.
		bool isCompiled;
};


class C_GLVertexShader : public C_GLShaderObject {
	public:
		C_GLVertexShader(void);
		~C_GLVertexShader(void);
};


class C_GLFragmentShader : public C_GLShaderObject {
	public:
		C_GLFragmentShader(void);
		~C_GLFragmentShader(void);
};


class C_GLShader {
	public:
		C_GLShader(void);
		~C_GLShader(void);

		// Add a vertex or fragment shader
		void AddShader(C_GLShaderObject* shader);

		// Link shaders
		bool Link(void);
		// Start using shader
		void Begin(void);
		// Stop using shader
		void End(void);

		inline bool GetisLinked(void) { return isLinked; }

		// Log returned after linking
		string linkerLog;

		// Uniform Variables
		bool setUniform1f(char* varname, GLfloat v0);  //!< set float uniform to program
		bool setUniform2f(char* varname, GLfloat v0, GLfloat v1); //!< set vec2 uniform to program
		bool setUniform3f(char* varname, GLfloat v0, GLfloat v1, GLfloat v2); //!< set vec3 uniform to program
		bool setUniform4f(char* varname, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3); //!< set vec4 uniform to program

		bool setUniform1i(char* varname, GLint v0);
		bool setUniform2i(char* varname, GLint v0, GLint v1);
		bool setUniform3i(char* varname, GLint v0, GLint v1, GLint v2);
		bool setUniform4i(char* varname, GLint v0, GLint v1, GLint v2, GLint v3);

		bool setUniform1fv(char* varname, GLsizei count, GLfloat *value);
		bool setUniform2fv(char* varname, GLsizei count, GLfloat *value);
		bool setUniform3fv(char* varname, GLsizei count, GLfloat *value);
		bool setUniform4fv(char* varname, GLsizei count, GLfloat *value);
		bool setUniform1iv(char* varname, GLsizei count, GLint *value);
		bool setUniform2iv(char* varname, GLsizei count, GLint *value);
		bool setUniform3iv(char* varname, GLsizei count, GLint *value);
		bool setUniform4iv(char* varname, GLsizei count, GLint *value);

		bool setUniformMatrix2fv(char* varname, GLsizei count, GLboolean transpose, GLfloat *value);
		bool setUniformMatrix3fv(char* varname, GLsizei count, GLboolean transpose, GLfloat *value);
		bool setUniformMatrix4fv(char* varname, GLsizei count, GLboolean transpose, GLfloat *value);

		GLenum bindAttribLocation(const char *varname, const GLint position);
		GLint getAttribLocation(const char *varname);

		/// Receive Uniform variables:
		void GetUniformfv(char* name, GLfloat* values);
		void GetUniformiv(char* name, GLint* values);

		void printAttribInfo(GLint attrib);

	private:
		// Holds all the shaders
		vector<C_GLShaderObject*> shaderList;

		GLuint programObject;

		GLint GetUniLoc(const GLcharARB *name);


		// Set to true if linking was successfull
		bool isLinked;

		bool inUse;
};


/*
C_GLShaderManager was written to easy the procedure of loading, compiling and linking vertex and fragment
shader files into shader objects ready for use. A typical use of this class is as follows :

C_GLShader* myShader;
C_GLShaderManager shaderManager;
myShader = shaderManager.LoadShaderProgram ( "shader.vert" , "shader.frag" );


// Draw function
myShader->Begin ();
// Draw something
myShader->End ();
*/

class C_GLShaderManager {
	public :
		C_GLShaderManager();
		~C_GLShaderManager();

		// Load a vertex and a fragment shader and returns a pointer to a C_GLShader object that holds both
		C_GLShader* LoadShaderProgram(const char* vertexFile , const char* fragmentFile);

	private:
		vector<C_GLShader*> shaderList;
};

//Initializes extensions using glew
bool InitGLExtensions(void);
//Checks if glsl can be used
bool CheckGLSL(void);


#endif
