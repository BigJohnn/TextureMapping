#version 450 core

in vec4 vert;

//uniform mat4 mvp;
out float intensity;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(void)
{
	intensity = vert.w;
	gl_Position = projection * view * model * vec4(vert.xyz, 1.0f);
}