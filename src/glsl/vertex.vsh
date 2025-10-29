#version 330 core
layout (location = 0) in vec3 positionCoordinate;
layout (location = 1) in vec2 textureCoordinate;
layout (location = 2) in vec3 normal;
//layour (location = 3) in vec3 normal;

out vec2 TexCoord;
uniform mat4 umat4Local;
uniform mat4 umat4Model;
uniform mat4 umat4View;
uniform mat4 umat4Proj;

void main()
{
	vec2 fixedcoord = vec2(textureCoordinate.x,1.0-textureCoordinate.y);
	gl_Position = umat4Proj*umat4View*umat4Model*umat4Local*vec4(positionCoordinate, 1.0f);
	TexCoord = fixedcoord;
}