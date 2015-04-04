uniform float time;
varying vec3 N, H;
uniform mat4 lightMVP, objectMVP;
varying vec4 shadowCoord;

void main( ) {
	shadowCoord = lightMVP * objectMVP * gl_Vertex * 0.5 + 0.5;
	vec4 W = gl_ModelViewMatrix * gl_Vertex;
	W.x += sin( gl_ModelViewMatrix[3][0] * 0.01 + time ) * gl_Vertex.z * 0.002;
	N = normalize(gl_NormalMatrix * gl_Normal);
	//H = normalize( normalize(-vec3(W)) + vec3( 0.57735 ) );
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = gl_ProjectionMatrix * W;
} 
