varying vec2 v_texCoords;
varying vec3 v_lightVec_es;
varying vec3 v_normals_es;

uniform sampler2D u_texture_diffuse;

vec4 lightColorDiffuse = vec4(1.0f, 1.0f, 1.0f, 1.0f);

void main(void)
{
   vec4 diffuseColor = texture2D(u_texture_diffuse, v_texCoords);

   vec4 Idiff = lightColorDiffuse * max(dot(v_normals_es, v_lightVec_es), 0.0);
   Idiff = clamp(Idiff, 0.0, 1.0);

   gl_FragColor = Idiff * diffuseColor;
}

