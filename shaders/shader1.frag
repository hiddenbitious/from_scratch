varying vec2 v_texCoords;
varying vec3 v_lightVec;
varying vec3 v_halfVec;

uniform sampler2D u_texture_diffuse;
uniform sampler2D u_texture_normal_map;
//uniform sampler2D u_texture_specular;

vec4 lightColorDiffuse  = vec4(227.0f / 255.0f,
                               140.0f / 255.0f,
                               45.0f / 255.0f, 1.0f);

vec4 lightColorSpecular = vec4(1.0f, 1.0f, 1.0f, 1.0f);

void main(void) {
   vec3 normal = 2.0 * texture2D (u_texture_normal_map, v_texCoords).rgb - 1.0;
	normal = normalize (normal);

	// compute diffuse lighting
	float lamberFactor = max(dot(v_lightVec, normal), 0.0);

//	vec4 specularMaterial = texture2D(u_texture_specular, v_texCoords);
	float shininess = pow (max (dot (v_halfVec, normal), 0.0), 2.0);

   vec4 diffuseMaterial = texture2D(u_texture_diffuse, v_texCoords);

   gl_FragColor = diffuseMaterial * lightColorDiffuse * lamberFactor;
//   gl_FragColor += specularMaterial * lightColorSpecular * shininess;
}
