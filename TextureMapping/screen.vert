#version 450 core
// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 p;
layout(location = 1) in vec2 texCoord;
// Output data ; will be interpolated for each fragment.
out vec2 UV;

void main(){
	gl_Position =  vec4(p, 1.0);
	UV = texCoord;
	//UV = (p.xy + vec2(1.0, 1.0)) / 2.0;
}