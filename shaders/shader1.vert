attribute vec3 a_vertices;
attribute vec3 a_normals;
attribute vec3 a_tangents;
attribute vec3 a_binormals;
attribute vec2 a_texCoords;

uniform mat4 u_modelviewMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_mvpMatrix;

uniform vec3 u_lightPosition;

varying vec2 v_texCoords;
varying vec3 lightVec;

void main(void) {
   vec3 t = vec3(normalize(u_modelviewMatrix * vec4(a_tangents, 0.0)));
   vec3 b = vec3(normalize(u_modelviewMatrix * vec4(a_binormals, 0.0)));
   vec3 n = vec3(normalize(u_modelviewMatrix * vec4(a_normals, 0.0)).xyz);

   vec3 vertexPosition = vec3(u_modelviewMatrix * vec4(a_vertices, 1.0));
   vec3 light = vec3(u_viewMatrix * vec4(u_lightPosition, 1.0));
	vec3 lightDir = normalize(light - vertexPosition);

	vec3 v;
	v.x = dot (lightDir, t);
	v.y = dot (lightDir, b);
	v.z = dot (lightDir, n);
	lightVec = normalize (v);

   v_texCoords = a_texCoords;
   gl_Position = u_mvpMatrix * vec4(a_vertices, 1.0);
}
