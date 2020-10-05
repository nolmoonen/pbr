// vertex shader
// use with: 'default.frag'
#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTex;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in mat4 instanceMatrix;

out vec3 normal;
out vec2 tex;

void main(){
    gl_Position = projectionMatrix * viewMatrix * instanceMatrix * vec4(inPosition, 1);
    normal = inNormal;
    tex = inTex;
}
