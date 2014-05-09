varying vec2 v_texCoords;
varying vec3 v_lightVec_es;
varying vec3 vertexPosition_es;
varying vec3 v_normals_es;

uniform sampler2D u_texture_diffuse;
uniform vec3 u_lightPosition_es;

//vec4 lightColorDiffuse = vec4(1.0f, 1.0f, 1.0f, 1.0f);
vec4 lightColorDiffuse  = vec4(255.0f / 255.0f,
                               170.0f / 255.0f,
                               105.0f / 255.0f, 1.0f);

float lightPower = 20.0;

void main(void)
{
   float distance = length(u_lightPosition_es - vertexPosition_es);

   vec4 diffuseColor = texture2D(u_texture_diffuse, v_texCoords);
   float lambert = clamp(dot(v_normals_es, v_lightVec_es), 0.0, 1.0);
//   vec4 Idiff = lambert * diffuseColor

   gl_FragColor = diffuseColor * lightColorDiffuse * lambert * lightPower / (distance);
}

