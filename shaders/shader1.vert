attribute vec4 a_vertices;
attribute vec3 a_normals;
attribute vec2 a_texCoords;

uniform mat4 u_modelviewMatrix;
uniform mat4 u_projectionMatrix;

varying vec2 v_texCoords;
varying vec4 v_normals;

void main(void) {
   mat4 mvp = u_projectionMatrix * u_modelviewMatrix;

   gl_Position = mvp * a_vertices;
   v_texCoords = a_texCoords;
//   v_normals = u_modelviewMatrix * vec4(a_normals.x, a_normals.y, a_normals.z, 0.0);
   v_normals = u_modelviewMatrix * vec4(a_normals, 0.0);
}
