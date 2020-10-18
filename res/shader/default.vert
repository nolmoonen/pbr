// vertex shader
// rendering colored primitives without shading
// use with: 'default.frag'
#version 330 core

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

layout (location = 0) in vec3 in_position;

void main(){
    gl_Position = projection_matrix * view_matrix * model_matrix * vec4(in_position, 1);
}