// vertex shader
// use with: 'lines.frag'
#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColor;

out vec3 color;

void main() {
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(inPosition, 1);
    color = inColor;
}
