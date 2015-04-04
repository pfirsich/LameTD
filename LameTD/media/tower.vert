varying vec3 N, H;
uniform mat4 lightMVP, objectMVP;
varying vec4 shadowCoord;

void main( ) {
	shadowCoord = lightMVP * objectMVP * gl_Vertex * 0.5 + 0.5;
	N = normalize(gl_NormalMatrix * gl_Normal);
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = ftransform( );
}  
