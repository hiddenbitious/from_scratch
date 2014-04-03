attribute vec4 a_vertices;
attribute vec4 a_normals;
attribute vec4 a_tangents;
attribute vec4 a_binormals;
attribute vec2 a_texCoords;

uniform mat4 u_modelviewMatrix;
uniform mat4 u_mvpMatrix;

uniform vec3 u_lightPosition;

varying vec2 v_texCoords;
varying vec3 lightVec;

void main(void) {
   vec3 t = vec3(normalize(u_modelviewMatrix * a_tangents));
   vec3 b = vec3(normalize(u_modelviewMatrix * a_binormals));
   vec3 n = vec3(normalize(u_modelviewMatrix * a_normals));

   vec3 vertexPosition = vec3(u_modelviewMatrix * a_vertices);
	vec3 lightDir = normalize(u_lightPosition - vertexPosition);

	vec3 v;
	v.x = dot (lightDir, t);
	v.y = dot (lightDir, b);
	v.z = dot (lightDir, n);
	lightVec = normalize (v);

   gl_Position = u_mvpMatrix * a_vertices;
   v_texCoords = a_texCoords;
//   v_normals = u_modelviewMatrix * vec4(a_normals, 0.0);
}
