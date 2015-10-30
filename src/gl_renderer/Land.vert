varying float _diffuseReflection; // Мощность отраженнного света которая будет передаваться во фрагментный шейдер

struct lightSource
{
  vec4 direction;
};
lightSource light0 = lightSource( vec4( 0.0, 0.0, 1.0, 0.0) );
 
void main(void) 
{
	vec3 normalDirection = normalize( vec3( gl_NormalMatrix * gl_Normal ) );
	vec3 lightDirection = normalize( vec3( light0.direction) );
	_diffuseReflection = max( 0.0, dot( normalDirection, lightDirection ) );
	gl_TexCoord[0] = gl_MultiTexCoord0;
	
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; 
}