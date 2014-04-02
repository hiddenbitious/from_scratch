attribute vec4 a_vertices;
attribute vec3 a_normals;
attribute vec3 a_tangents;
attribute vec3 a_binormals;
attribute vec2 a_texCoords;

uniform mat4 u_modelviewMatrix;
uniform mat4 u_projectionMatrix;
uniform mat4 u_mvpMatrix;

uniform vec3 u_lightPosition;

varying vec2 v_texCoords;
varying vec4 v_normals;

void main(void) {
//   vec3 n = normalize(u_modelviewMatrix * a_normals);


   gl_Position = u_mvpMatrix * a_vertices;
   v_texCoords = a_texCoords;
   v_normals = u_modelviewMatrix * vec4(a_normals, 0.0);
}
