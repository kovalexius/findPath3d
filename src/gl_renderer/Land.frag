varying float _diffuseReflection;

//out vec4 fragColor;

void main(void) 
{
	float koef1 = ( mod( gl_TexCoord[0].x, 0.13 ) + mod( gl_TexCoord[0].y, 0.13 ) );
	float koef2 = ( mod( gl_TexCoord[0].x, 0.07 ) + mod( gl_TexCoord[0].y, 0.07 ) );
    vec3 color = vec3( 0.1, koef1 * 0.3 + koef2 * 0.4 + 0.3, 0.2 );
	gl_FragColor = vec4( color * _diffuseReflection, 1.0 );
}