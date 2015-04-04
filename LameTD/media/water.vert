varying vec3 N, P, E;
uniform vec3 camPos;
uniform mat4 lightMVP;
varying vec4 shadowCoord;

void main( ) {
	shadowCoord = lightMVP * gl_Vertex * 0.5 + 0.5;
	N = normalize(gl_NormalMatrix * gl_Normal);
	P = camPos - gl_Vertex.xyz;
	E = vec3(gl_ModelViewMatrix * gl_Vertex);
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = ftransform( );
}  
