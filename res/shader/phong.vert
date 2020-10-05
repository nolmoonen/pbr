// vertex shader
// use with: 'phong.frag'
#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTex;
layout (location = 2) in vec3 inNormal;

out vec3 normal;
out vec3 pos;
out vec2 tex;

void main(){
    vec4 modelpos = modelMatrix * vec4(inPosition, 1);
    pos = modelpos.xyz;
    gl_Position = projectionMatrix * viewMatrix * modelpos;
    normal = inNormal;
    tex = inTex;
}