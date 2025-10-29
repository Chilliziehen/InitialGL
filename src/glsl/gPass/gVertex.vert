#version 430
layout (location = 0) in vec3 positionCoordinate;
layout (location = 1) in vec2 textureCoordinate;
layout (location = 2) in vec3 normal;

out vec2 texCoord;
out vec3 worldCoord;
out vec3 fragNormal;

uniform mat4 umat4Model;
uniform mat4 umat4View;
uniform mat4 umat4Proj;

void main() {
    texCoord = textureCoordinate;
    vec4 worldPosition = umat4Model * vec4(positionCoordinate, 1.0f);
    worldCoord = worldPosition.xyz;
    fragNormal = mat3(transpose(inverse(umat4Model))) * normal; // 法线变换
    gl_Position = umat4Proj * umat4View * worldPosition;
}