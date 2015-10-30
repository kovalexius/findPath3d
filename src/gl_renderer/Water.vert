uniform float time;
varying float _diffuseReflection; // Мощность отраженнного света которая будет передаваться во фрагментный шейдер
varying float _time;

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
	_time = time;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_TexCoord[0].x += abs(_time-0.5)/4.0;
	
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; 
}