varying vec2 v_texCoords;
varying vec4 v_normals;

uniform sampler2D u_texture_0;

void main(void) {
//   vec2 coords = vec2 (2.0f * tmp.x + v_texCoords.x, 3.0f * tmp.y + v_texCoords.y);
   gl_FragColor = texture2D(u_texture_0, v_texCoords) * normalize(v_normals);
//   gl_FragColor = normalize(v_normals);
}
