varying vec2 v_texCoords;
uniform sampler2D u_texture_0;
uniform vec2 tmp;

void main(void) {
//   gl_FragColor = vec4(0.9f, 0.8f, 0.95f, 1.f);
//   vec2 coords = vec2 (2.0f * tmp.x + v_texCoords.x, 3.0f * tmp.y + v_texCoords.y);
   gl_FragColor = texture2D(u_texture_0, v_texCoords);
}
