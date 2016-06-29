#version 450 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 position;

// Values that stay constant for the whole mesh.
uniform mat4 depthMVP;
uniform mat4 model;

void main(){
	gl_Position =  depthMVP *model* vec4(position, 1);
}
