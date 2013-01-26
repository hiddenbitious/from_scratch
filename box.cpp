#include "box.h"



box::box(void)
{
	color_dif[0] = 0.4f;	color_dif[1] = 0.74f;	color_dif[2] = 1.0f;	color_dif[3] = 1.0f;
	color_spec[0] = 1.0f;	color_spec[1] = 1.0f;	color_spec[2] = 1.0f;	color_spec[3] = 1.0f;
	color_amb[0] = 0.0f;	color_amb[1] = 0.0f;	color_amb[2] = 0.0f;	color_amb[3] = 1.0f;

	shader = NULL;
}

bool box::LoadShaders(void)
{
	shader = shaderManager.LoadShaderProgram("box_texSpec.vert" , "box_texSpec.frag");

	return true;
}

bool box::LoadTextures(const char* name)
{
//	TextureLoader texLoader;
	bool textureLoaded = false;

//	textureLoaded = texLoader.LoadTextureFromDisk ( name , &texture );
//
//	if ( textureLoaded )
//	{
//		cout << "Texture " << name << " loaded ok.\n";
//	}
//	else
//		cout << "Error loading texture " << name << endl;

	return textureLoaded;
}

void box::Rotate(const float anglex , const float angley , const float anglez)
{
	C_Quaternion tempQuat;
	tempQuat.Rotate(anglex , angley , anglez);

	//Aplly rotation
	rotationQuaternion.Mult(&tempQuat);
	//Rotate BVOlumes
	RotateBVolumes(anglex , angley , anglez , &position);
}

void box::Rotate(const float anglex , const float angley , const float anglez , const C_Vector3* rotPoint)
{
	C_Quaternion tempQuat;
	tempQuat.Rotate(anglex , angley , anglez);

	//Aplly rotation
	rotationQuaternion.Mult(&tempQuat);
	//Rotate BVOlumes
	RotateBVolumes(anglex , angley , anglez , rotPoint);
}

void box::RotateBVolumes(const float anglex , const float angley , const float anglez , const C_Vector3* rotPoint)
{
	bbox.Rotate(anglex , angley , anglez , rotPoint);
}


void box::Draw(void)
{
	_PushTransMatrix();

	rotationQuaternion.QuaternionToAxisAngle(x , y , z , a);
	_ApplyTranslation(position.x , position.y , position.z);
	_ApplyRotation(a , x , y , z);

	shader->Begin();
	// Load texture dimensions to shader
//			shader->setUniform1f ( "textureWidth" , (float) texture.Width );
//			shader->setUniform1f ( "textureHeight" , (float) texture.Height );

	glMaterialfv(GL_FRONT , GL_DIFFUSE , color_dif);
	glMaterialfv(GL_FRONT , GL_AMBIENT , color_amb);
	glMaterialfv(GL_FRONT , GL_SPECULAR , color_spec);

//			glTexGeni ( GL_S , GL_TEXTURE_GEN_MODE , GL_SPHERE_MAP );
//			glTexGeni ( GL_T , GL_TEXTURE_GEN_MODE , GL_SPHERE_MAP );
//			glEnable ( GL_TEXTURE_GEN_S );
//			glEnable ( GL_TEXTURE_GEN_T );

	glBindTexture(GL_TEXTURE_2D , texture.TextureID);
//			glEnable ( GL_TEXTURE_2D );

	glMateriali(GL_FRONT , GL_SHININESS , 70);
	glutSolidCube(70.0);
	float tmp;
	//shader->GetUniformfv ( "textureWidth" , &tmp );
	shader->End();

	_PopTransMatrix();
}


void box::Translate(const float x , const float y , const float z)
{
	position.Translate(x , y , z);
	bbox.Translate(x , y , z);
}

void box::Translate(const C_Vector3 *vec)
{
	position.Translate(vec);
	bbox.Translate(vec);
}
