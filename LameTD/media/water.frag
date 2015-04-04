uniform sampler2D base, normal, base2;
uniform float time;

varying vec3 P, N, E;

const float blendDist = 50.0;

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

	gl_TexCoord[0].xy *= 10.0;

	N = texture2D( normal, gl_TexCoord[0].xy + time * 0.05 ).xyz * 2.0 - 1.0;
	N = normalize(gl_NormalMatrix * N);
	E = normalize( E );

	vec3 refl = reflect( -E, N );
	vec4 reflColor = texture2D( base, gl_TexCoord[0].xy * 0.5 + refl.xz / refl.y * 0.05 );

	vec3 refr = refract( -E, N, 0.75 ); 
	vec4 refrColor = texture2D( base2, gl_TexCoord[0].xy * 4.0 + refr.xz / refr.y * 0.03 );

	vec3 lightRefl = reflect( vec3( 0.5, 0.0, 0.866 ), N );
	vec4 specular = pow( max( dot( lightRefl, E ), 0.0 ), 20.0 ) * vec4( 2.0 );

	float cosTheta = 1.0 - max( 0.0, dot( N, -E ) );
	float fresnel = cosTheta * cosTheta;
	fresnel = fresnel * fresnel * cosTheta;
	fresnel = fresnel * 0.6 + 0.4;
	gl_FragColor = (mix( refrColor, reflColor, fresnel ) + specular) * shadow;
}  
