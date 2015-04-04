uniform sampler2D base;
uniform sampler2DShadow shadowMap;
varying vec3 P, N;
varying vec4 shadowCoord;
const float blendDist = 50.0;
const float off = 1.0 / SHADOWMAPSIZE;

float sample( vec2 offset ) {
	return shadow2DProj( shadowMap, shadowCoord + vec4( offset, -0.005, 0.0 ) );
}

void main( ) {
	#ifdef PCF
		float shadow = 0.111111 * (sample( vec2( 0.0, 0.0 ) ) + sample( vec2( off, 0.0 ) ) + sample( vec2( off, -off ) ) + sample( vec2( 0.0, -off ) ) + 
		sample( vec2( -off, -off ) ) + sample( vec2( -off, 0.0 ) ) + sample( vec2( -off, off ) ) + sample( vec2( 0.0, off ) ) + sample( vec2( off, off ) ) );
	#else
		float shadow = sample( vec2( 0.0, 0.0 ) );
	#endif
	shadow += 0.2;

	float dist = length( P );
	vec3 albedo = texture2D( base, gl_TexCoord[0].xy * 20.0 ).rgb;
	if( dist < blendDist ) albedo = mix( albedo, texture2D( base, gl_TexCoord[0].xy * 100.0 ), (blendDist - dist) / blendDist );

	gl_FragColor = vec4( (0.1 + max( dot( N, vec3( 0.5, 0.0, 0.866 ) ), 0.0 )) * albedo * shadow, 1.0 );
}  
