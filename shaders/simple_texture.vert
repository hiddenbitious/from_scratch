attribute vec3 a_vertices;
attribute vec3 a_normals;
attribute vec2 a_texCoords;

uniform mat4 u_modelviewMatrix;
uniform mat4 u_mvpMatrix;

uniform vec3 u_lightPosition_es;

varying vec2 v_texCoords;
varying vec3 v_lightVec_es;
varying vec3 vertexPosition_es;
varying vec3 v_normals_es;

void main(void) {
   vertexPosition_es = vec3(u_modelviewMatrix * vec4(a_vertices, 1.0));

   v_lightVec_es = normalize(u_lightPosition_es - vertexPosition_es);

   v_texCoords = a_texCoords;
   v_normals_es = vec3((u_modelviewMatrix * vec4(a_normals, 0.0)));
   gl_Position = u_mvpMatrix * vec4(a_vertices, 1.0);
}
