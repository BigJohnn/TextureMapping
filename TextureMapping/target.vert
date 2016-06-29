#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec4 ShadowCoord;
out vec3 Position;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 DepthBiasMVP;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);
	Position = (model * vec4(position, 1.0f)).xyz;
    TexCoords = texCoords;
	Normal = transpose(inverse(mat3(model))) * normal;//normal;

	ShadowCoord = DepthBiasMVP * vec4(position,1.0);
	
}