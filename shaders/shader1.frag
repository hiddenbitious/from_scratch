varying vec2 v_texCoords;
varying vec3 lightVec;

uniform sampler2D u_texture_0;
uniform sampler2D u_texture_1;

vec4 lightColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);

void main(void) {
   vec3 normal = 2.0 * texture2D (u_texture_1, v_texCoords).rgb - 1.0;
	normal = normalize (normal);

	// compute diffuse lighting
	float lamberFactor = max(dot(lightVec, normal), 0.0);

   vec4 diffuseMaterial = texture2D(u_texture_0, v_texCoords);
   gl_FragColor = diffuseMaterial * lightColor * lamberFactor;
}
