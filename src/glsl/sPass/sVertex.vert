#version 430

layout (location = 0) in vec3 positionCoordinate;

uniform mat4 umat4Model;
uniform mat4 umat4LightView;
uniform mat4 umat4LightProj;

void main() {
    gl_Position = umat4LightProj * umat4LightView * umat4Model * vec4(positionCoordinate, 1.0f);
}