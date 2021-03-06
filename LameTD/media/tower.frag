uniform sampler2D base;
varying vec3 H, N;

uniform sampler2DShadow shadowMap;
varying vec4 shadowCoord;
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

	gl_FragColor = vec4( vec3( (0.1 + max( dot( N, lightDir ), 0.0 )) * shadow ), 1.0 );
} 
