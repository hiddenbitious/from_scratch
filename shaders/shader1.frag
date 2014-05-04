varying vec2 v_texCoords;
varying vec3 v_lightVec_ts;
varying vec3 v_eyeDirection_ts;
varying vec3 v_vertexPosition_es;

uniform sampler2D u_texture_diffuse;
uniform sampler2D u_texture_normal_map;
uniform sampler2D u_texture_specular;

uniform vec3 u_lightPosition_es;

vec4 lightColorDiffuse  = vec4(255.0f / 255.0f,
                               170.0f / 255.0f,
                               105.0f / 255.0f, 1.0f);

//vec4 lightColorDiffuse = vec4(1.0f, 1.0f, 1.0f, 1.0f);
vec4 lightColorSpecular = vec4(1.0f, 1.0f, 1.0f, 1.0f);
float lightPower = 20.0;

void main(void) {
   vec3 normal_ts = normalize(2.0 * texture2D (u_texture_normal_map, v_texCoords).rgb - 1.0);

	/// compute diffuse lighting
	vec3 l = normalize(v_lightVec_ts);
	float lamberFactor = clamp(dot(l, normal_ts), 0.0, 1.0);

   vec4 diffuseColor = texture2D(u_texture_diffuse, v_texCoords);
	vec4 specularColor = texture2D(u_texture_specular, v_texCoords);
	vec4 ambientColor = vec4(0.01 ,0.01, 0.01, 1.0) * diffuseColor;

   vec3 E = normalize(v_eyeDirection_ts);
   vec3 R = reflect(-l,normal_ts);
   float cosAlpha = clamp( dot( E,R ), 0.0, 1.0);
   float distance = length(u_lightPosition_es - v_vertexPosition_es);

   gl_FragColor = ambientColor +
                  diffuseColor * lightColorDiffuse * lamberFactor * lightPower / (distance) +
                  specularColor * lightColorSpecular * pow(cosAlpha, 5) * lightPower / distance;
//   gl_FragColor += specularMaterial * lightColorSpecular * shininess;
}
