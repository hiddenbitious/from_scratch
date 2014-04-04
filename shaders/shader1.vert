attribute vec3 a_vertices;
attribute vec3 a_normals;
attribute vec3 a_tangents;
attribute vec3 a_binormals;
attribute vec2 a_texCoords;

uniform mat4 u_modelviewMatrix;
uniform mat4 u_mvpMatrix;

uniform vec3 u_lightPosition_eyeSpace;

varying vec2 v_texCoords;
varying vec3 v_lightVec;
varying vec3 v_halfVec;

void main(void) {
   vec3 t = vec3(normalize(u_modelviewMatrix * vec4(a_tangents, 0.0)));
   vec3 b = vec3(normalize(u_modelviewMatrix * vec4(a_binormals, 0.0)));
   vec3 n = vec3(normalize(u_modelviewMatrix * vec4(a_normals, 0.0)).xyz);

   vec3 vertexPosition = vec3(u_modelviewMatrix * vec4(a_vertices, 1.0));
//   vec3 light = vec3(u_viewMatrix * vec4(u_lightPosition, 1.0));
	vec3 lightDir = normalize(u_lightPosition_eyeSpace - vertexPosition);

	vec3 v;
	v.x = dot (lightDir, t);
	v.y = dot (lightDir, b);
	v.z = dot (lightDir, n);
	v_lightVec = normalize (v);

   // No need to divide by two, the result is normalized anyway.
	// vec3 halfVector = normalize((vertexPosition + lightDir) / 2.0);
	vertexPosition = normalize(vertexPosition);
	vec3 halfVector = normalize(vertexPosition + lightDir);
	v.x = dot (halfVector, t);
	v.y = dot (halfVector, b);
	v.z = dot (halfVector, n);

	// No need to normalize, t,b,n and halfVector are normal vectors.
	//normalize (v);
	v_halfVec = v ;

   v_texCoords = a_texCoords;
   gl_Position = u_mvpMatrix * vec4(a_vertices, 1.0);
}
