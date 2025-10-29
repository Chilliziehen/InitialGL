#version 330 core
in vec2 TexCoord;

out vec4 color;

uniform sampler2D textureUniform1;
// 其余纹理保留占位，便于后续拓展但当前未用
uniform sampler2D textureUniform2;
uniform sampler2D textureUniform3;
uniform sampler2D textureUniform4;
uniform sampler2D textureUniform5;
uniform sampler2D textureUniform6;

// 可选环境光强度，未设置时默认为 0.1
uniform float ambientStrength;

void main()
{
    vec3 albedo = texture(textureUniform1, TexCoord).rgb;
    color = vec4(albedo, 1.0);
}