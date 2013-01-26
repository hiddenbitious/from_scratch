#ifndef _BOX_H_


#include "globals.h"
#include "quaternion.h"
#include "frustum.h"
#include "vectors.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/glut.h>

#include "../oglRenderer/oglDriver.h"
#include "../oglRenderer/shaders/glsl.h"
//#include "../oglRenderer/textureLoader/TextureLoader.h"

class box {
	private:

		float scalex , scaley , scalez;
		C_Vector3 position;
		C_Quaternion rotationQuaternion;

		C_BBox bbox;
		float color_dif[4];
		float color_amb[4];
		float color_spec[4];


		// Texture
		glTexture texture;


		//ZOMG SHADERS!!1!
		C_GLShaderManager shaderManager;
		C_GLShader* shader;

		// Used in the draw method to convert the quaternion into a rotation matrix
		float rotationMatrix[16];
		float x, y, z, a;



		void TranslateBVolumes(const float x , const float y , const float z);
		void TranslateBVolumes(const C_Vector3* vec);
		void RotateBVolumes(const float x , const float y , const float z , const C_Vector3* rotPoint);
		void RotateBVolumes(const C_Quaternion* quat , const C_Vector3* rotPoint);


	public:
		box(void);
		bool LoadShaders(void);
		bool LoadTextures(const char *name);

		void SetPosition(const float x , const float y , const float z);
		void SetPosition(const C_Vector3* vec);

		void Rotate(const float anglex , const float angley , const float anglez);
		void Rotate(const C_Quaternion* quat);
		void Rotate(const float anglex , const float angley , const float anglez , const C_Vector3* rotPoint);
		void Rotate(const C_Quaternion* quat , const C_Vector3* rotPoint);

		// Translate
		void Translate(const float x , const float y , const float z);
		// Translate
		void Translate(const C_Vector3* vec);



		void Draw(void);
		int Draw(C_Frustum* frustum);
};


#endif
