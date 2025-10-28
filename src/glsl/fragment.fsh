#version 430 core
in vec3 verColor;
in vec2 TexCoord;
in vec3 actualLight;
out vec4 color;
uniform sampler2D textureUniform1;
uniform sampler2D textureUniform2;
uniform sampler2D textureUniform3;
uniform sampler2D textureUniform4;
uniform sampler2D textureUniform5;
uniform sampler2D textureUniform6;
void main()
{
    color = texture(textureUniform1,TexCoord);
    gl_Color.x = 1.0f;
    gl_Color.y = 1.0f;
    gl_Color.z = 1.0f;
    //color.x = color.x*actualLight.x;
    //color.y = color.y*actualLight.y;
    //color.z = color.z*actualLight.z;
}