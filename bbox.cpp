#include "bbox.h"
#include <stdio.h>

const char *C_BBox::ADJACENT_FACE_NAMES[TOTAL_FACES] = { "X_MINUS",
                                                   "Y_MINUS",
                                                   "Z_MINUS",
                                                   "X_PLUS",
                                                   "Y_PLUS",
                                                   "Z_PLUS" };

C_BBox::C_BBox()
{
}

C_BBox::~C_BBox()
{
}

C_BBox & C_BBox::operator= (const C_BBox &box)
{
   if (this != &box) {
      min = box.min;
      max = box.max;

      for(int i = 0; i < 8; i++) {
         vertices[i] = box.vertices[i];
      }
   }

   return *this;
}

void C_BBox::SetMax(const float x , const float y , const float z)
{
	max.x = x;
	max.y = y;
	max.z = z;
}

void C_BBox::SetMax(const C_Vector3 *vec)
{
	max.SetVector(*vec);
}

void C_BBox::SetMin(const float x , const float y , const float z)
{
	min.x = x;
	min.y = y;
	min.z = z;
}

void C_BBox::SetMin(const C_Vector3 *vec)
{
	min.SetVector(*vec);
}

void C_BBox::SetVertices(void)
{
	vertices[0].SetVector(max.x, min.y, min.z);
	vertices[1].SetVector(max.x, min.y, max.z);
	vertices[2].SetVector(min.x, min.y, max.z);
	vertices[3].SetVector(min.x, min.y, min.z);

	vertices[4].SetVector(max.x, max.y, min.z);
	vertices[5].SetVector(max.x, max.y, max.z);
	vertices[6].SetVector(min.x, max.y, max.z);
	vertices[7].SetVector(min.x, max.y, min.z);
}

void C_BBox::GetVertices(C_Vector3* _vertices) const
{
	_vertices[0].SetVector(max.x, min.y, min.z);
	_vertices[1].SetVector(max.x, min.y, max.z);
	_vertices[2].SetVector(min.x, min.y, max.z);
	_vertices[3].SetVector(min.x, min.y, min.z);

	_vertices[4].SetVector(max.x, max.y, min.z);
	_vertices[5].SetVector(max.x, max.y, max.z);
	_vertices[6].SetVector(min.x, max.y, max.z);
	_vertices[7].SetVector(min.x, max.y, min.z);
}

void C_BBox::GetVertices(C_Vertex* _vertices) const
{
	_vertices[0].x = max.x;		_vertices[0].y = min.y;		_vertices[0].z = min.z;
	_vertices[1].x = max.x;		_vertices[1].y = min.y;		_vertices[1].z = max.z;
	_vertices[2].x = min.x;		_vertices[2].y = min.y;		_vertices[2].z = max.z;
	_vertices[3].x = min.x;		_vertices[3].y = min.y;		_vertices[3].z = min.z;

	_vertices[4].x = max.x;		_vertices[4].y = max.y;		_vertices[4].z = min.z;
	_vertices[5].x = max.x;		_vertices[5].y = max.y;		_vertices[5].z = max.z;
	_vertices[6].x = min.x;		_vertices[6].y = max.y;		_vertices[6].z = max.z;
	_vertices[7].x = min.x;		_vertices[7].y = max.y;		_vertices[7].z = min.z;
}

void C_BBox::GetMax(C_Vector3* vec)
{
	vec->SetVector(max);
}

void C_BBox::GetMin(C_Vector3* vec)
{
	vec->SetVector(min);
}

void C_BBox::GetMax(C_Vertex* vec)
{
	vec->x = max.x;
	vec->y = max.y;
	vec->z = max.z;
}

void C_BBox::GetMin(C_Vertex* vec)
{
	vec->x = min.x;
	vec->y = min.y;
	vec->z = min.z;
}

void C_BBox::GetMax(float* x , float* y , float* z)
{
	*x = max.x;
	*y = max.y;
	*z = max.z;
}

void C_BBox::GetMin(float* x , float* y , float* z)
{
	*x = min.x;
	*y = min.y;
	*z = min.z;
}

void C_BBox::Draw(void)
{
   Draw(1.0f, 1.0f, 1.0f);
}

void C_BBox::Draw(float r , float g , float b)
{
   int polygonMode[2];
   glGetIntegerv(GL_POLYGON_MODE, polygonMode);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

   C_Vertex verts[24];
	//Bottom
	verts[0].x = min.x, verts[0].y = min.y, verts[0].z = max.z;
	verts[1].x = min.x, verts[1].y = min.y, verts[1].z = min.z;
	verts[2].x = max.x, verts[2].y = min.y, verts[2].z = min.z;
	verts[3].x = max.x, verts[3].y = min.y, verts[3].z = max.z;

	//Top
	verts[4].x = min.x, verts[4].y = max.y, verts[4].z = max.z;
	verts[5].x = min.x, verts[5].y = max.y, verts[5].z = min.z;
	verts[6].x = max.x, verts[6].y = max.y, verts[6].z = min.z;
	verts[7].x = max.x, verts[7].y = max.y, verts[7].z = max.z;

	//Left
	verts[8].x = min.x, verts[8].y = min.y, verts[8].z = min.z;
	verts[9].x = min.x, verts[9].y = min.y, verts[9].z = max.z;
	verts[10].x = min.x, verts[10].y = max.y, verts[10].z = max.z;
	verts[11].x = min.x, verts[11].y = max.y, verts[11].z = min.z;

	//Right
	verts[12].x = max.x, verts[12].y = min.y, verts[12].z = min.z;
	verts[13].x = max.x, verts[13].y = min.y, verts[13].z = max.z;
	verts[14].x = max.x, verts[14].y = max.y, verts[14].z = max.z;
	verts[15].x = max.x, verts[15].y = max.y, verts[15].z = min.z;

	//Front
	verts[16].x = min.x, verts[16].y = min.y, verts[16].z = max.z;
	verts[17].x = min.x, verts[17].y = max.y, verts[17].z = max.z;
	verts[18].x = max.x, verts[18].y = max.y, verts[18].z = max.z;
	verts[19].x = max.x, verts[19].y = min.y, verts[19].z = max.z;

	//Back
	verts[20].x = min.x, verts[20].y = min.y, verts[20].z = min.z;
	verts[21].x = min.x, verts[21].y = max.y, verts[21].z = min.z;
	verts[22].x = max.x, verts[22].y = max.y, verts[22].z = min.z;
	verts[23].x = max.x, verts[23].y = min.y, verts[23].z = min.z;

   shaderManager->pushShader(basicShader);
      basicShader->setUniform4f("u_v4_color", r, g, b, 1.0f);

      glEnableVertexAttribArray(basicShader->verticesAttribLocation);

      basicShader->setUniformMatrix4fv(UNIFORM_VARIABLE_NAME_VIEW_MATRIX, 1, GL_FALSE, (GLfloat *)&globalViewMatrix.m[0][0]);
      basicShader->setUniformMatrix4fv(UNIFORM_VARIABLE_NAME_PROJECTION_MATRIX, 1, GL_FALSE, (GLfloat *)&globalProjectionMatrix.m[0][0]);

      glEnableVertexAttribArray(basicShader->verticesAttribLocation);
      glVertexAttribPointer(basicShader->verticesAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, verts);
      glDrawArrays(GL_QUADS, 0, 24);
   shaderManager->popShader();

	glPolygonMode(GL_FRONT_AND_BACK, polygonMode[0]);
}

void C_BBox::Translate(const float x, const float y, const float z)
{
	min.Translate(x , y , z);
	max.Translate(x , y , z);

	vertices[0].Translate(x , y , z);
	vertices[1].Translate(x , y , z);
	vertices[2].Translate(x , y , z);
	vertices[3].Translate(x , y , z);
	vertices[4].Translate(x , y , z);
	vertices[5].Translate(x , y , z);
	vertices[6].Translate(x , y , z);
	vertices[7].Translate(x , y , z);
}

void C_BBox::Translate(const C_Vector3 *vec)
{
	min.Translate(vec);
	max.Translate(vec);

	vertices[0].Translate(vec);
	vertices[1].Translate(vec);
	vertices[2].Translate(vec);
	vertices[3].Translate(vec);
	vertices[4].Translate(vec);
	vertices[5].Translate(vec);
	vertices[6].Translate(vec);
	vertices[7].Translate(vec);
}

void C_BBox::Rotate(const float anglex , const float angley , const float anglez , const C_Vector3 *rotPoint)
{
	C_Quaternion tempQuat;
	tempQuat.Rotate(anglex , angley , anglez);

	tempQuat.RotatePoint(&vertices[0] , rotPoint);
	tempQuat.RotatePoint(&vertices[1] , rotPoint);
	tempQuat.RotatePoint(&vertices[2] , rotPoint);
	tempQuat.RotatePoint(&vertices[3] , rotPoint);
	tempQuat.RotatePoint(&vertices[4] , rotPoint);
	tempQuat.RotatePoint(&vertices[5] , rotPoint);
	tempQuat.RotatePoint(&vertices[6] , rotPoint);
	tempQuat.RotatePoint(&vertices[7] , rotPoint);

	max.x = MaxOfEight(vertices[0].x , vertices[1].x , vertices[2].x , vertices[3].x , vertices[4].x , vertices[5].x , vertices[6].x , vertices[7].x);
	max.y = MaxOfEight(vertices[0].y , vertices[1].y , vertices[2].y , vertices[3].y , vertices[4].y , vertices[5].y , vertices[6].y , vertices[7].y);
	max.z = MaxOfEight(vertices[0].z , vertices[1].z , vertices[2].z , vertices[3].z , vertices[4].z , vertices[5].z , vertices[6].z , vertices[7].z);

	min.x = MinOfEight(vertices[0].x , vertices[1].x , vertices[2].x , vertices[3].x , vertices[4].x , vertices[5].x , vertices[6].x , vertices[7].x);
	min.y = MinOfEight(vertices[0].y , vertices[1].y , vertices[2].y , vertices[3].y , vertices[4].y , vertices[5].y , vertices[6].y , vertices[7].y);
	min.z = MinOfEight(vertices[0].z , vertices[1].z , vertices[2].z , vertices[3].z , vertices[4].z , vertices[5].z , vertices[6].z , vertices[7].z);
}

void
C_BBox::ApplyTransformation(ESMatrix *matrix)
{
   for(int i = 0; i < 8; ++i) {
      vertices[i] = math::transformPoint(matrix, &vertices[i]);
   }

   max.x = MaxOfEight(vertices[0].x , vertices[1].x , vertices[2].x , vertices[3].x , vertices[4].x , vertices[5].x , vertices[6].x , vertices[7].x);
	max.y = MaxOfEight(vertices[0].y , vertices[1].y , vertices[2].y , vertices[3].y , vertices[4].y , vertices[5].y , vertices[6].y , vertices[7].y);
	max.z = MaxOfEight(vertices[0].z , vertices[1].z , vertices[2].z , vertices[3].z , vertices[4].z , vertices[5].z , vertices[6].z , vertices[7].z);

	min.x = MinOfEight(vertices[0].x , vertices[1].x , vertices[2].x , vertices[3].x , vertices[4].x , vertices[5].x , vertices[6].x , vertices[7].x);
	min.y = MinOfEight(vertices[0].y , vertices[1].y , vertices[2].y , vertices[3].y , vertices[4].y , vertices[5].y , vertices[6].y , vertices[7].y);
	min.z = MinOfEight(vertices[0].z , vertices[1].z , vertices[2].z , vertices[3].z , vertices[4].z , vertices[5].z , vertices[6].z , vertices[7].z);
}

float C_BBox::MaxOfEight(float n1 , float n2 , float n3 , float n4 , float n5 , float n6 , float n7 , float n8)
{
	float max = n1;

	if(n2 > max) { max = n2; }
	if(n3 > max) { max = n3; }
	if(n4 > max) { max = n4; }
	if(n5 > max) { max = n5; }
	if(n6 > max) { max = n6; }
	if(n7 > max) { max = n7; }
	if(n8 > max) { max = n8; }

	return max;
}

float C_BBox::MinOfEight(float n1 , float n2 , float n3 , float n4 , float n5 , float n6 , float n7 , float n8)
{
	float min = n1;

	if(n2 < min) { min = n2; }
	if(n3 < min) { min = n3; }
	if(n4 < min) { min = n4; }
	if(n5 < min) { min = n5; }
	if(n6 < min) { min = n6; }
	if(n7 < min) { min = n7; }
	if(n8 < min) { min = n8; }

	return min;
}

bool C_BBox::IsInside(const C_Vertex* p)
{
//	if ( p->x >= min.x && p->x <= max.x && p->y >= min.y && p->y <= max.y && p->z >= min.z && p->z <= max.z )
//		 return true;

   if(FLOAT_SMALLER(p->x, min.x) || FLOAT_GREATER(p->x, max.x)) {
      return false;
   }

   if(FLOAT_SMALLER(p->y, min.y) || FLOAT_GREATER(p->y, max.y)) {
      return false;
   }

   if(FLOAT_SMALLER(p->z, min.z) || FLOAT_GREATER(p->z, max.z)) {
      return false;
   }

//	if(p->x < min.x || p->x > max.x) {
//		return false;
//	}
//	if(p->y < min.y || p->y > max.y) {
//		return false;
//	}
//	if(p->z < min.z || p->z > max.z) {
//		return false;
//	}

	return true;
}

/// XXX: Bad hard float comparing goes on in this function
adjacent_face_t
C_BBox::areBboxesAdjacent(const C_BBox *box)
{
   if(FLOAT_EQ(min.x, box->max.x) &&
      ((min.z >= box->min.z && min.z <= box->max.z) ||
      (box->min.z >= min.z && box->min.z <= max.z))) {
      return X_MINUS;
   }

   if(FLOAT_EQ(max.x, box->min.x) &&
      ((min.z >= box->min.z && min.z <= box->max.z) ||
      (box->min.z >= min.z && box->min.z <= max.z))) {
      return X_PLUS;
   }

   if(FLOAT_EQ(min.z, box->max.z) &&
      ((min.x >= box->min.x && min.x <= box->max.x) ||
      (box->min.x >= min.x && box->min.x <= max.x))) {
      return Z_MINUS;
   }

   if(FLOAT_EQ(max.z, box->min.z) &&
      ((min.x >= box->min.x && min.x <= box->max.x) ||
      (box->min.x >= min.x && box->min.x <= max.x))) {
      return Z_PLUS;
   }

   return TOTAL_FACES;
}
