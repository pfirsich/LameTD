varying vec3 N, H;
uniform mat4 lightMVP, objectMVP;
varying vec4 shadowCoord;

void main( ) {
	shadowCoord = lightMVP * objectMVP * gl_Vertex * 0.5 + 0.5;
	N = normalize(gl_NormalMatrix * gl_Normal);
	gl_TexCoord[0] = gl_MultiTexCoord0;
	//H = normalize( normalize(-vec3(gl_ModelViewMatrix * gl_Vertex)) + vec3( 0.57735 ) );
	gl_Position = ftransform( );
} 
