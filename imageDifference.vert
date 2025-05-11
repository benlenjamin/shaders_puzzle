// make this 120 for the mac:
#version 330 compatibility

layout (location = 3) in vec3 Color;

out vec4 OriginalColor;

uniform mat4 ModelViewProjectionMatrix;

// out variables to be interpolated in the rasterizer and sent to each fragment shader:

out  vec2  vST;	  // (s,t) texture coordinates
out  vec3  vN;	  // normal vector
out  vec3  vL;	  // vector from point to light
out  vec3  vE;	  // vector from point to eye

const vec3 LIGHTPOSITION = vec3( 0., 5., 5. );
void
main( )
{
	vST = gl_MultiTexCoord0.st;
	vec4 ECposition = gl_ModelViewMatrix * gl_Vertex; // eye coordinate position
	vN = normalize( gl_NormalMatrix * gl_Normal ); // normal vector
	vL = LIGHTPOSITION - ECposition.xyz; // vector from the point to the light position
	vE = vec3( 0., 0., 0. ) - ECposition.xyz; // vector from the point to the eye position 
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	//gl_Position = ModelViewProjectionMatrix * vec4(Vertex, 1.0);
	OriginalColor = vec4(Color, 1.0);
}
