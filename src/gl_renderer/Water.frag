varying float _diffuseReflection;
varying float _time;

//out vec4 fragColor;

void main(void) 
{
	float koef1 = ( mod( gl_TexCoord[0].x, 0.07 ) + mod( gl_TexCoord[0].y, 0.07 ) );
	float koef2 = ( mod( gl_TexCoord[0].x, 0.02 ) + mod( gl_TexCoord[0].y, 0.02 ) );
	vec3 color = vec3( 0.1, 0.1, koef1 * 0.6 + koef2 * 0.8 + 0.1 );
	gl_FragColor = vec4( color * _diffuseReflection, 0.7 + abs(_time-0.5)/2.0 );
}