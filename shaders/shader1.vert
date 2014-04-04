attribute vec3 a_vertices;
attribute vec3 a_normals;
attribute vec3 a_tangents;
attribute vec3 a_binormals;
attribute vec2 a_texCoords;

uniform mat4 u_modelviewMatrix;
//uniform mat4 u_modelMatrix;
uniform mat4 u_mvpMatrix;

uniform vec3 u_lightPosition_es;

varying vec2 v_texCoords;
varying vec3 v_lightVec_ts;
varying vec3 v_eyeDirection_ts;
varying vec3 v_vertexPosition_es;

vec3 v;

void main(void) {
   vec3 t = vec3(u_modelviewMatrix * vec4(a_tangents, 0.0));
   vec3 b = vec3(u_modelviewMatrix * vec4(a_binormals, 0.0));
   vec3 n = vec3(u_modelviewMatrix * vec4(a_normals, 0.0));

   vec3 vertexPosition_es = vec3(u_modelviewMatrix * vec4(a_vertices, 1.0));
	vec3 lightDir_es = (u_lightPosition_es - vertexPosition_es);

//   v_vertexPosition_ws = (u_modelMatrix * vec4(a_vertices, 1.0)).xyz;
   v_vertexPosition_es = vertexPosition_es;

   /// Light direction
	v.x = dot (lightDir_es, t);
	v.y = dot (lightDir_es, b);
	v.z = dot (lightDir_es, n);
	v_lightVec_ts =  v;

	/// Eye direction
	vertexPosition_es = -vertexPosition_es;
   v.x = dot (vertexPosition_es, t);
	v.y = dot (vertexPosition_es, b);
	v.z = dot (vertexPosition_es, n);
	v_eyeDirection_ts = v;

   v_texCoords = a_texCoords;
   gl_Position = u_mvpMatrix * vec4(a_vertices, 1.0);
}
