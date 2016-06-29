#version 450 core

in vec2 TexCoords;
in vec3 Normal;
in vec4 ShadowCoord;
in vec3 Position;

out vec4 color;

uniform sampler2D texture_diffuse;
uniform sampler2DShadow shadowMap;

uniform vec3 imgNorm;

uniform vec3 camPos;

int t = 0;

void main()
{    
	//float bias = 0.005*tan(acos(dot(Normal, normalize(camPos-Position)))); // cosTheta is dot( n,l ), clamped between 0 and 1
	//bias = clamp(bias, 0.0,0.01);

	//float visibility=0;

	//if(ShadowCoord.z>0)
	//{
	//	visibility = textureProj( shadowMap, ShadowCoord)  <  (ShadowCoord.z-bias)?1:0;
	//}
	if(dot(Normal, imgNorm) < t)
	{
		color =  vec4(texture(texture_diffuse, TexCoords));// *vec4(visibility);
	}
}