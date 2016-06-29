#version 450 core

in vec2 UV;
// Ouput data
layout(location = 0) out vec4 color;

uniform sampler2D screenTexture;

void main(){
	float Depth = texture(screenTexture, UV).x;
    //Depth = 1.0 - (1.0 - Depth) * 25.0;
    color = vec4(Depth);
	//color = vec4(texture(screenTexture, UV));
}