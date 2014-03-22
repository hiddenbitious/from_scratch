attribute vec4 a_vertices;
attribute vec2 a_texCoords;

uniform mat4 u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;

varying vec2 v_texCoords;

void main(void) {
//   mat4 mvp = u_projectionMatrix * u_modelviewMatrix;
   mat4 mv = u_viewMatrix * u_modelMatrix;
   mat4 mvp = u_projectionMatrix * mv;
   gl_Position = mvp * a_vertices;
   v_texCoords = a_texCoords;
}
