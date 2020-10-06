// vertex shader
// use with: 'pbr.frag'
#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTex;
layout (location = 2) in vec3 inNormal;

out vec3 geom;
out vec2 tex;
out vec3 normal;

void main()
{
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(inPosition, 1);
    geom = gl_Position.xyz;
    tex = inTex;
    normal = inNormal;
}