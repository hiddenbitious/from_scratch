#include "glsl.h"

//#ifndef JNI_COMPATIBLE
#	include <fstream>
//#endif

bool extensions_init = false;
bool glslAvailable = false;
bool C_GLShaderManager::instanceFlag = false;
C_GLShaderManager *C_GLShaderManager::classInstance = NULL;

char g_glMajorVersion;
char g_glMinorVersion;

//vector<C_GLShader *> C_GLShaderManager::shaderList;

C_GLShaderObject::C_GLShaderObject(void)
{
   PRINT_FUNC_ENTRY;

	type = NO_SHADER;
	shaderObject = 0;
	sourceBytes = 0;
	isCompiled = false;
	shaderSource = NULL;
	compilerLog = NULL;
}

C_GLShaderObject::~C_GLShaderObject(void)
{
   PRINT_FUNC_ENTRY;

	if(shaderSource != NULL) {
		delete[] shaderSource;
	}

	if(compilerLog != NULL) {
		delete[] compilerLog;
	}

	if(isCompiled) {
		glDeleteObjectARB(shaderObject);
	}
}

#ifndef JNI_COMPATIBLE
static unsigned long getFileLength(ifstream& file)
{
	if(!file.good()) {
		return 0;
	}

	file.seekg(0 , ios::end);
	unsigned long len = file.tellg();
	file.seekg(ios::beg);

	return len;
}
#endif

#ifndef JNI_COMPATIBLE
bool C_GLShaderObject::LoadShaderProgram(const char* filename)
{
	ifstream file;
	file.open(filename, ios::in);
	if(!file) {
		return false;
	}

	sourceBytes = getFileLength(file);

	// "Empty File"
	if(!sourceBytes) {
		return false;
	}

	// there is already a source loaded, free it!
	if(shaderSource != NULL) {
		delete[] shaderSource;
	}

	// can't reserve memory
	shaderSource = (GLubyte*) new char[sourceBytes + 1];
	if(!shaderSource) {
		return false;
	}

	// sourceBytes isn't always strlen cause some characters are stripped in ascii read...
	// it is important to 0-terminate the real length later, sourceBytes is just max possible value...
	shaderSource[sourceBytes] = '\0';

	unsigned int i = 0;
	while(file.good()) {
		shaderSource[i++] = file.get();

		// coding guidelines...
		if(i > sourceBytes) {
			i = sourceBytes;
		}
	}

	// 0 terminate it. Also delete the last char (255)
	shaderSource[i - 1] = '\0';
//	this->fileName = filename;
	file.close();

	return true;
}
#else
bool C_GLShaderObject::LoadShaderProgram(const char* shaderSource)
{
	int shaderSourceLength = strlen(shaderSource);

	if(!shaderSourceLength)
		return false;

	if(this->shaderSource) {
		delete[] this->shaderSource;
	}

	this->shaderSource = (GLubyte*) new char[shaderSourceLength + 1];
	if(!this->shaderSource) {
		return false;
	}

	memcpy((void *)this->shaderSource, (void *)shaderSource, shaderSourceLength);
	this->shaderSource[shaderSourceLength] = '\0';

	return true;
}
#endif
//-------------------------------------------------------------
bool C_GLShaderObject::compile(bool printSource)
{
	if(!glslAvailable) {
		return false;
	}

	if(isCompiled) {
	   return true;
	}

	#ifndef JNI_COMPATIBLE
	if(printSource) {
		cout << endl << endl << shaderSource << endl << endl;
	}
	#endif

	glShaderSource(shaderObject , 1 , (const GLchar **)&shaderSource , NULL);

	/// Compile shader
	int success = 0;
	glCompileShader(shaderObject);
	glGetShaderiv(shaderObject , GL_COMPILE_STATUS , &success);

	/// Get log size
	int logSize = 0;
	glGetShaderiv(shaderObject , GL_INFO_LOG_LENGTH , &logSize);
	compilerLog = new char[logSize];

	glGetShaderInfoLog(shaderObject, logSize, NULL, compilerLog);

	isCompiled = (bool)!!success;

	return (bool)!!success;
}

C_GLVertexShader::C_GLVertexShader(void)
{
   PRINT_FUNC_ENTRY;

	type = VERTEX_SHADER;

	if(glslAvailable) {
		shaderObject = glCreateShader(GL_VERTEX_SHADER);
	}
}

C_GLVertexShader::~C_GLVertexShader(void)
{
   PRINT_FUNC_ENTRY;

   glDeleteShader(shaderObject);
}

C_GLFragmentShader::C_GLFragmentShader(void)
{
   PRINT_FUNC_ENTRY;

	type = FRAGMENT_SHADER;

	if(glslAvailable) {
		shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	}
}

C_GLFragmentShader::~C_GLFragmentShader(void)
{
   PRINT_FUNC_ENTRY;

   glDeleteShader(shaderObject);
}
//-------------------------------------------------------------

//-------------------------------------------------------------
C_GLShader::C_GLShader(void)
{
   PRINT_FUNC_ENTRY;

	programObject = 0;
	isLinked = false;
	inUse = false;
	nShaders = 0;
	linkerLog = NULL;

	verticesAttribLocation = -1;
	normalsAttribLocation = -1;
	texCoordsAttribLocation = -1;
	colorsAttribLocation = -1;

	if(glslAvailable) {
		programObject = glCreateProgram();
	}
}
//-------------------------------------------------------------
C_GLShader::~C_GLShader(void)
{
   PRINT_FUNC_ENTRY;

	if(glslAvailable) {
		for(int i = 0; i < nShaders; i++) {
			glDetachShader(programObject , shaderList[i]->shaderObject);
			delete shaderList[i];
		}
		nShaders = 0;
		glDeleteProgram(programObject);
	}
}
//-------------------------------------------------------------

//-------------------------------------------------------------

void C_GLShader::AddShader(C_GLShaderObject* shader)
{
	if(!glslAvailable) {
		return;
	}

	if(shader == NULL) {
		return;
	}

	if(!shader->isCompiled) {
		LOGI("Warning. Shader is not compiled. Attempting to compile now.\n");
		if(!shader->compile(false)) {
			return;
		}
	}

   assert(nShaders < MAX_SHADERS);
	shaderList[nShaders++] = shader;
}

bool C_GLShader::Link(void)
{
	if(!glslAvailable || !nShaders) {
		return false;
	}

	int i;

	if(isLinked) {
		LOGI("Object program already linked. Attempting to link again.\n");

		for(i = 0 ; i < nShaders; i++) {
			glDetachShader(programObject , shaderList[i]->shaderObject);
		}
	}

	// Attach all the shaders
	for(i = 0 ; i < nShaders; i++) {
		glAttachShader(programObject , shaderList[i]->shaderObject);
	}

	// Link the shader programs
	glLinkProgram(programObject);

	int success;
	glGetProgramiv(programObject , GL_LINK_STATUS , &success);

	if(!success) {
		//Get log size
		int logSize = 0;
		glGetShaderiv(programObject , GL_INFO_LOG_LENGTH , &logSize);
		linkerLog = new char[logSize];

		glGetProgramInfoLog(programObject , logSize , NULL , linkerLog);
		return false;
	}

	isLinked = true;
	return true;
}

void C_GLShader::Begin(void)
{
	if(!programObject)	{ return; }
	if(!isLinked) { return; }
	if(inUse) { return; }

	inUse = true;

	glUseProgram(programObject);
}

void C_GLShader::End(void)
{
	if(!programObject)	{ return; }
	if(!inUse) { return; }

	inUse = false;

	glUseProgram(0);
}

GLenum C_GLShader::bindAttribLocation(const char *varname, const GLint position)
{
	if(!glslAvailable) {
		return false;
	}

//	GLint numActiveAttribs;
//	glGetProgramiv(programObject, GL_ACTIVE_ATTRIBUTES, &numActiveAttribs);
//
//	cout << "~!~ " << numActiveAttribs << endl;

	GLenum glError;

	glBindAttribLocation(programObject, position, varname);
	glError = glGetError();
	if(glError != GL_NO_ERROR) {
		return glError;
	}

	/// Binding must be done before linking. Link again!
	if(!this->Link()) {
		LOGE("Failed to link shader.\n");
		return -1;
	}

	return GL_NO_ERROR;
}

GLint C_GLShader::getAttribLocation(const char *varname)
{
	if(!glslAvailable || !isLinked) {
		return -2;
	}

	GLint tmp = glGetAttribLocation(programObject, varname);
	return tmp;
}

void C_GLShader::printAttribInfo(GLint attrib)
{
	if(!glslAvailable || attrib < 0)
		return;

	char name[64];
	GLsizei length;
	GLint size;
	GLenum type;

	glGetActiveAttrib(programObject, attrib, 64, &length, &size, &type, name);

	LOGI("Attribute: %d info:\n", attrib);
	LOGI("\tName length: %d\n", length);
	LOGI("\tsize: %d\n", size);
	LOGI("\ttype: 0x%x\n", type);
	LOGI("\tname: %s\n", name);
}

bool C_GLShader::setUniform1f(const char* varname, GLfloat v0)
{
	if(!glslAvailable) { return false; }  // GLSL not available
//    if (!_noshader) return true;

	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform1f(loc, v0);

	return true;
}

bool C_GLShader::setUniform2f(const char* varname, GLfloat v0, GLfloat v1)
{
	if(!glslAvailable) { return false; }  // GLSL not available
//   if (!_noshader) return true;

	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform2f(loc, v0, v1);

	return true;
}

bool C_GLShader::setUniform3f(const char* varname, GLfloat v0, GLfloat v1, GLfloat v2)
{
	if(!glslAvailable) { return false; }  // GLSL not available
//    if (!_noshader) return true;

	GLint loc = GetUniLoc(varname);
//	if(loc == -1) { assert(0); return false; } // can't find variable

	glUniform3f(loc, v0, v1, v2);

	return true;
}

bool C_GLShader::setUniform4f(const char* varname, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
	if(!glslAvailable) { return false; }  // GLSL not available
//    if (!_noshader) return true;

	GLint loc = GetUniLoc(varname);
	if(loc == -1) { assert(0); return false; } // can't find variable

	glUniform4f(loc, v0, v1, v2, v3);

	return true;
}

bool C_GLShader::setUniform1i(const char* varname, GLint v0)
{
	if(!glslAvailable) { return false; }  // GLSL not available
//    if (!_noshader) return true;

	GLint loc = GetUniLoc(varname);
//	if(loc == -1) { assert(0); return false; } // can't find variable

	glUniform1i(loc, v0);

	return true;
}

bool C_GLShader::setUniform2i(const char* varname, GLint v0, GLint v1)
{
	if(!glslAvailable) { return false; }  // GLSL not available
//    if (!_noshader) return true;

	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform2i(loc, v0, v1);


	return true;
}

bool C_GLShader::setUniform3i(const char* varname, GLint v0, GLint v1, GLint v2)
{
	if(!glslAvailable) { return false; }  // GLSL not available
//    if (!_noshader) return true;

	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform3i(loc, v0, v1, v2);

	return true;
}

bool C_GLShader::setUniform4i(const char* varname, GLint v0, GLint v1, GLint v2, GLint v3)
{
	if(!glslAvailable) { return false; }  // GLSL not available
//    if (!_noshader) return true;

	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform4i(loc, v0, v1, v2, v3);

	return true;
}

bool C_GLShader::setUniform1fv(const char* varname, GLsizei count, GLfloat *value)
{
	if(!glslAvailable) { return false; }  // GLSL not available
//    if (!_noshader) return true;

	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform1fv(loc, count, value);

	return true;
}

bool C_GLShader::setUniform2fv(const char* varname, GLsizei count, GLfloat *value)
{
	if(!glslAvailable) { return false; }  // GLSL not available
//    if (!_noshader) return true;

	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform2fv(loc, count, value);

	return true;
}

bool C_GLShader::setUniform3fv(const char* varname, GLsizei count, GLfloat *value)
{
	if(!glslAvailable) { return false; }  // GLSL not available
//    if (!_noshader) return true;

	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform3fv(loc, count, value);

	return true;
}

bool C_GLShader::setUniform4fv(const char* varname, GLsizei count, GLfloat *value)
{
	if(!glslAvailable) { return false; }  // GLSL not available
//    if (!_noshader) return true;

	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform4fv(loc, count, value);

	return true;
}

bool C_GLShader::setUniform1iv(const char* varname, GLsizei count, GLint *value)
{
	if(!glslAvailable) { return false; }  // GLSL not available
//    if (!_noshader) return true;

	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform1iv(loc, count, value);

	return true;
}

bool C_GLShader::setUniform2iv(const char* varname, GLsizei count, GLint *value)
{
	if(!glslAvailable) { return false; }  // GLSL not available
//    if (!_noshader) return true;

	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform2iv(loc, count, value);

	return true;
}

bool C_GLShader::setUniform3iv(const char* varname, GLsizei count, GLint *value)
{
	if(!glslAvailable) { return false; }  // GLSL not available
//    if (!_noshader) return true;

	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform3iv(loc, count, value);

	return true;
}

bool C_GLShader::setUniform4iv(const char* varname, GLsizei count, GLint *value)
{
	if(!glslAvailable) { return false; }  // GLSL not available
//    if (!_noshader) return true;

	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform4iv(loc, count, value);

	return true;
}

bool C_GLShader::setUniformMatrix2fv(const char* varname, GLsizei count, GLboolean transpose, GLfloat *value)
{
	if(!glslAvailable) { return false; }  // GLSL not available
//    if (!_noshader) return true;

	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniformMatrix2fv(loc, count, transpose, value);

	return true;
}

bool C_GLShader::setUniformMatrix3fv(const char* varname, GLsizei count, GLboolean transpose, GLfloat *value)
{
	if(!glslAvailable) { return false; }  // GLSL not available
//    if (!_noshader) return true;

	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniformMatrix3fv(loc, count, transpose, value);

	return true;
}

bool C_GLShader::setUniformMatrix4fv(const char* varname, GLsizei count, GLboolean transpose, GLfloat *value)
{
	if(!glslAvailable) { assert(0); return false; }  // GLSL not available
//    if (!_noshader) return true;

	GLint loc = GetUniLoc(varname);
	if(loc == -1) { assert(0); return false; } // can't find variable

	glUniformMatrix4fv(loc, count, transpose, value);

	return true;
}

void C_GLShader::GetUniformfv(char* name, GLfloat* values)
{
	if(!glslAvailable) { return; }
	GLint loc;

	loc = glGetUniformLocation(programObject, name);
	if(loc == -1) {
		LOGE("Error: can't find uniform variable \"%s\"\n", name);
	}

	glGetUniformfv(programObject, loc, values);
}

void C_GLShader::GetUniformiv(char* name, GLint* values)
{
	if(!glslAvailable) { return; }


	GLint loc;

	loc = glGetUniformLocation(programObject, name);
	if(loc == -1) {
		LOGE("Error: can't find uniform variable \"%s\"\n", name);
	}

	glGetUniformiv(programObject, loc, values);
}

void C_GLShader::UpdateAttribLocations(void)
{
   assert(isLinked);

   verticesAttribLocation     = getAttribLocation(VERTEX_ATTRIBUTE_VARIABLE_NAME_VERTICES);
   normalsAttribLocation      = getAttribLocation(VERTEX_ATTRIBUTE_VARIABLE_NAME_NORMALS);
   texCoordsAttribLocation    = getAttribLocation(VERTEX_ATTRIBUTE_VARIABLE_NAME_TEXCOORDS);
   colorsAttribLocation       = getAttribLocation(VERTEX_ATTRIBUTE_VARIABLE_NAME_COLORS);
   tangetsAttribLocation      = getAttribLocation(VERTEX_ATTRIBUTE_VARIABLE_NAME_TANGENTS);
   binormalsAttribLocation    = getAttribLocation(VERTEX_ATTRIBUTE_VARIABLE_NAME_BINORMALS);

   textureUniformLocation_0   = GetUniLoc(UNIFORM_VARIABLE_NAME_TEXTURE_0);
   textureUniformLocation_1   = GetUniLoc(UNIFORM_VARIABLE_NAME_TEXTURE_1);
   textureUniformLocation_2   = GetUniLoc(UNIFORM_VARIABLE_NAME_TEXTURE_2);

   textureDiffuseLocation_0   = GetUniLoc(UNIFORM_VARIABLE_NAME_TEXTURE_DIFFUSE);
   textureNormalMapLocation_1 = GetUniLoc(UNIFORM_VARIABLE_NAME_TEXTURE_NORMAL_MAP);
   textureSpecularLocation_2  = GetUniLoc(UNIFORM_VARIABLE_NAME_TEXTURE_SPECULAR);
}

C_GLShaderManager::C_GLShaderManager(void)
{
   PRINT_FUNC_ENTRY;

//	shaderList = NULL;
}

C_GLShaderManager::~C_GLShaderManager(void)
{
   PRINT_FUNC_ENTRY;

   for(unsigned int i = 0; i < shaderList.size(); ++i) {
      delete shaderList[i];
   }
   shaderList.clear();
}

C_GLShaderManager *C_GLShaderManager::getSingleton(void)
{
   if(!instanceFlag) {
      classInstance = new C_GLShaderManager();
      instanceFlag = true;
   }

   return classInstance;
}

/// Finds if a shader has already been loaded by comparing it's source
C_GLShaderObject *C_GLShaderManager::shaderObjectExists(const C_GLShaderObject *shaderObject, shader_type_t type)
{
   C_GLShaderObject *currentShader;

   for(unsigned int i = 0; i < shaderList.size(); i++) {
      for(int j = 0; j < shaderList[i]->nShaders; j++) {
         currentShader = shaderList[i]->shaderList[j];
         if(type != currentShader->type)
            continue;

         if(!memcmp(shaderObject->shaderSource, currentShader->shaderSource, currentShader->sourceBytes) &&
            shaderObject->sourceBytes == currentShader->sourceBytes)
            return currentShader;
      }
   }

   return NULL;
}

C_GLShader *C_GLShaderManager::shaderExists(const C_GLShaderObject *shaderObject1, const C_GLShaderObject *shaderObject2)
{
   for(unsigned int i = 0; i < shaderList.size(); i++) {
      /// In case the shader object consists of only 1 shader
      if(shaderList[i]->nShaders == 1) {
         if(shaderList[i]->shaderList[0] == shaderObject1) {
            return shaderList[i];
         }
      } else if (shaderList[i]->nShaders == 2) {
         if((shaderList[i]->shaderList[0] == shaderObject1 && shaderList[i]->shaderList[1] == shaderObject2) ||
            (shaderList[i]->shaderList[0] == shaderObject2 && shaderList[i]->shaderList[1] == shaderObject1))
            return shaderList[i];
      }
   }

   return NULL;
}

void C_GLShaderManager::pushShader(C_GLShader *shader)
{
   /// XXX: Optimization: Check if shader being pushed is already at the
   ///      top of the stack.
   /// Deactivate current shader
   if(activeShader.size())
      activeShader[activeShader.size() - 1]->End();
   /// Place new shader on top of stack
   activeShader.push_back(shader);
   shader->Begin();
}

void C_GLShaderManager::popShader(void)
{
   if(!activeShader.size())
      return;

   /// XXX: Optimization: If shader stack is not empty skip calling End() on shader
   ///      that is on top of stack, and activate immediately next shader
   activeShader[activeShader.size() - 1]->End();
   activeShader.pop_back();
   if(activeShader.size())
      activeShader[activeShader.size() - 1]->Begin();
}

#ifndef JNI_COMPATIBLE
C_GLShader *
C_GLShaderManager::LoadShaderProgram(const char *vertexFile, const char *fragmentFile)
{
   bool fragmentExists = false;
   bool vertexExists = false;
   C_GLShaderObject *tmpShader;
	C_GLShader* shaderObject;
	C_GLVertexShader* tVertexShader;
	C_GLFragmentShader* tFragmentShader;

   cout.flush();
   fflush(stdout);
	printf("----------------------------------------------------------\n");
	printf("Loading shader ...\n");

	/// Load vertex shader
	tVertexShader = new C_GLVertexShader();
	printf("\tLoading vertex shader from file:   \"%s\"... ", vertexFile);
	if(!tVertexShader->LoadShaderProgram(vertexFile)) {
		printf("Failed: Error reading file.\n");
		delete tVertexShader;
		return NULL;
	}

	if((tmpShader = shaderObjectExists(tVertexShader, VERTEX_SHADER))) {
      vertexExists = true;
	   delete tVertexShader;
	   tVertexShader = (C_GLVertexShader *)tmpShader;
	   printf("Vertex shader already loaded.\n");
	} else {
   	printf("done!\n");
	}

	/// Load fragment shader
	tFragmentShader = new C_GLFragmentShader();
	printf("\tLoading fragment shader from file: \"%s\"... ", fragmentFile);
	if(!tFragmentShader->LoadShaderProgram(fragmentFile)) {
		printf("Failed: Error reading file.\n");
		delete tVertexShader;
		delete tFragmentShader;
		return NULL;
	}

	if((tmpShader = shaderObjectExists(tFragmentShader, FRAGMENT_SHADER))) {
      fragmentExists = true;
	   delete tFragmentShader;
	   tFragmentShader = (C_GLFragmentShader *)tmpShader;
	   printf("Fragment shader already loaded.\n");
	} else {
   	printf("done!\n");
	}

	/// Compile vertex shader
	printf("\tCompiling vertex shader...");
//	fflush(stdout);
	if(!tVertexShader->compile(false)) {
		printf("Error compiling vertex shader in file: %s\n\n", vertexFile);
		printf("Compiler log:\n");
		printf("%s\n", tVertexShader->compilerLog);
		delete tVertexShader;
		delete tFragmentShader;
		return NULL;
	} else {
	   printf(" done!\n");
   }

	if(strlen(tVertexShader->compilerLog)) {
		printf("Compiler log: \n");
		printf("%s\n", tVertexShader->compilerLog);
	}

	/// Compile fragment shader
	printf("\tCompiling fragment shader...");
	if(!tFragmentShader->compile(false)) {
		printf("Error compiling fragment shader in file: %s\n\n", fragmentFile);
		printf("Compiler log:\n");
		printf("%s\n", tFragmentShader->compilerLog);
		delete tVertexShader;
		delete tFragmentShader;
		return NULL;
	} else {
	   printf("done!\n");
   }

	if(strlen(tFragmentShader->compilerLog)) {
		printf("Compiler log:\n");
		printf("%s\n", tFragmentShader->compilerLog);
	}

	/// Add shaders to shader object
	if(fragmentExists && vertexExists) {
      shaderObject = shaderExists(tVertexShader, tFragmentShader);
      assert(shaderObject);
      return shaderObject;
	} else {
	   shaderObject = new C_GLShader();
      shaderObject->AddShader(tVertexShader);
      shaderObject->AddShader(tFragmentShader);
   }

	/// Link shader object
	printf("\tLinking shaders into shader object...");
	if(!shaderObject->Link()) {
		printf(" Error linking shader programs.\n");
		printf("%s\n\n", shaderObject->linkerLog);
		return shaderObject;
		delete tVertexShader;
		delete tFragmentShader;
	}

   printf("done!\n");
	printf("----------------------------------------------------------\n");

   /// Detect all default vertex attributes and shader uniforms
	shaderObject->UpdateAttribLocations();

   /// Put the new shader in the shader manager's list
	shaderList.push_back(shaderObject);

	return shaderObject;
}
#else
C_GLShader* C_GLShaderManager::LoadShaderProgram(const char *vertexSource , const char *fragmentSource)
{
	C_GLShader* shaderObject;
	C_GLVertexShader* tVertexShader;
	C_GLFragmentShader* tFragmentShader;

	LOGI("\n********************************************************************************\n\n");
	LOGI("Loading shader . . .\n");

	// Load vertex shader
	tVertexShader = new C_GLVertexShader();
	shaderObject = new C_GLShader();

	LOGI("Loading vertex shader...\n");
	if(!tVertexShader->LoadShaderProgram(vertexSource)) {
		LOGE("\nCan't load vertex shader \n");
		delete tVertexShader;
		return shaderObject;
	}
	LOGI("done!\n");

	// Load fragment shader
	tFragmentShader = new C_GLFragmentShader();

	LOGI("Loading fragment shader\n");
	if(!tFragmentShader->LoadShaderProgram(fragmentSource)) {
		LOGE("\nCan't load fragment shader\n");
		delete tVertexShader;
		delete tFragmentShader;
		return shaderObject;
	}
	LOGI("done!\n");

	// Compile vertex shader

	LOGI("Compiling vertex shader...\n");
	if(!tVertexShader->compile(true)) {
		LOGE("Error compiling vertex shader\n");
//		cout << "Compiler log: " << endl;
//		cout << tVertexShader->compilerLog << endl;
		delete tVertexShader;
		delete tFragmentShader;
		return shaderObject;
	}
	LOGI("...done!\n");
//	if(tVertexShader->compilerLog.size()) {
//		cout << "Compiler log: " << endl;
//		cout << tVertexShader->compilerLog << endl;
//	}

	// Compile fragment shader

	LOGI("Compiling fragment shader...\n");
	if(!tFragmentShader->compile(true)) {
		LOGE("Error compiling fragment shader\n");
//		cout << "Compiler log: " << endl;
//		cout << tFragmentShader->compilerLog << endl;
		delete tVertexShader;
		delete tFragmentShader;
		return shaderObject;
	}
	LOGI("...done!\n");
//	if(tFragmentShader->compilerLog.size()) {
//		cout << "Compiler log: " << endl;
//		cout << tFragmentShader->compilerLog << endl;
//	}

	// Add shaders to shader object
	shaderObject->AddShader(tVertexShader);
	shaderObject->AddShader(tFragmentShader);

	// Link shader object
//	cout << "----------------------------------------------------------" << endl;
	LOGI("Linking shader objects...");
	if(!shaderObject->Link()) {
		LOGE("\nError linking shader programs.\n");
//		cout << shaderObject->linkerLog << "\n\n";
		delete tVertexShader;
		delete tFragmentShader;
		return shaderObject;
	}
	LOGI("done!\n");
	LOGI("\n********************************************************************************\n\n");

//	shaderList.push_back(shaderObject);
	if(!shaderList)
		delete shaderList;
	shaderList = shaderObject;

	return shaderObject;
}
#endif

bool InitGLExtensions(void)
{
	if(extensions_init) {
		return true;
	}

	//Init GLEW
	GLenum err = glewInit();
	if(GLEW_OK != err) {
		//Problem: glewInit failed, something is seriously wrong.
		cout << "Error initizing GLEW." << endl << "Error: " << glewGetErrorString(err) << endl;
		extensions_init = false;
		return false;
	} else {
		cout << "GLEW initialized" << endl;
	}

	const GLubyte* oglVendor = glGetString(GL_VENDOR);
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* oglVersion = glGetString(GL_VERSION);

	cout << "OpenGL Version: " << oglVersion << endl;
	cout << "OpenGL Vendor: " << oglVendor << endl;
	cout << "Renderer: " << renderer << endl;

	return true;
}

#ifndef JNI_COMPATIBLE
bool CheckGLSL(void)
{
	if(glslAvailable) {
		return true;
	}

	glslAvailable = true;

	cout << "\n\n";

	if(GLEW_VERSION_4_1) {
        std::cout << "OpenGL 4.1 is available!" << std::endl;
        g_glMajorVersion = 4;
        g_glMinorVersion = 1;
    } else if(GLEW_VERSION_4_0) {
        std::cout << "OpenGL 4.0 is available!" << std::endl;
        g_glMajorVersion = 4;
        g_glMinorVersion = 0;
    } else if(GLEW_VERSION_3_3) {
        std::cout << "OpenGL 3.3 is available!" << std::endl;
        g_glMajorVersion = 3;
        g_glMinorVersion = 3;
    } else if(GLEW_VERSION_3_2) {
        std::cout << "OpenGL 3.2 is available!" << std::endl;
        g_glMajorVersion = 3;
        g_glMinorVersion = 2;
    } else if(GLEW_VERSION_3_1) {
        std::cout << "OpenGL 3.1 is available!" << std::endl;
        g_glMajorVersion = 3;
        g_glMinorVersion = 1;
    } else if(GLEW_VERSION_3_0) {
        std::cout << "OpenGL 3.0 is available!" << std::endl;
        g_glMajorVersion = 3;
        g_glMinorVersion = 0;
    } else if(GLEW_VERSION_2_1) {
        std::cout << "OpenGL 2.1 is available!" << std::endl;
        g_glMajorVersion = 2;
        g_glMinorVersion = 1;
    } else if(GLEW_VERSION_2_0) {
        std::cout << "OpenGL 2.0 is available!" << std::endl;
        g_glMajorVersion = 2;
        g_glMinorVersion = 0;
    }

	if(GL_TRUE != glewGetExtension("GL_ARB_fragment_shader")) {
		cout << "GL_ARB_fragment_shader extension is not available!";
		glslAvailable = false;
	} else {
		cout << "GL_ARB_fragment_shader extension is available!" << endl;
	}

	if(GL_TRUE != glewGetExtension("GL_ARB_vertex_shader")) {
		cout << "GL_ARB_vertex_shader extension is not available!" << endl;
		glslAvailable = false;
	} else {
		cout << "GL_ARB_vertex_shader extension is available!" << endl;
	}

	if(GL_TRUE != glewGetExtension("GL_ARB_shader_objects")) {
		cout << "GL_ARB_shader_objects extension is not available!" << endl;
		glslAvailable = false;
	} else {
		cout << "GL_ARB_shader_objects extension is available!" << endl;
	}

   /// This is needed for the glGenerateMipmap function
	if(GL_TRUE != glewGetExtension("GL_EXT_framebuffer_object")) {
		cout << "GL_EXT_framebuffer_object extension is not available!" << endl;
		glslAvailable = false;
	} else {
		cout << "GL_EXT_framebuffer_object extension is available!" << endl;
	}

	if(glslAvailable) {
		cout << "OpenGL Shading Language is available!\n\n";
	} else {
		cout << "OpenGL Shading Language is not available :(\n\n";
	}

	return glslAvailable;
}
#else
bool CheckGLSL(void)
{
	return glslAvailable = true;
}
#endif
