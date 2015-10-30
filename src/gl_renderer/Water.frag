varying float _diffuseReflection;
varying float _time;

//out vec4 fragColor;

void main(void) 
{
	float koef1 = ( mod( gl_TexCoord[0].x, 0.1 ) + mod( gl_TexCoord[0].y, 0.1 ) );
	float koef2 = ( mod( gl_TexCoord[0].x, 0.04 ) + mod( gl_TexCoord[0].y, 0.04 ) );
	vec3 color = vec3( 0.1, 0.1, koef1 * 0.6 + koef2 * 0.6 + 0.1 );
	gl_FragColor = vec4( color * _diffuseReflection, 0.6 + abs(_time-0.5)/1.8 );
}