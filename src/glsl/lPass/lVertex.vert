#version 430

layout(location = 0) in vec2 NDCPosition;       //[-1,1]
layout(location = 1) in vec2 textureCoordinate; //[0,1]

out vec2 tCoord;

void main() {
    gl_Position = vec4(NDCPosition, 0.0, 1.0);
    tCoord = textureCoordinate;
}