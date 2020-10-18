// vertex shader
// rendering colored lines primitives without lightning
// use with: 'lines.frag'
#version 330 core

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_color;

out vec3 color;

void main() {
    gl_Position = projection_matrix * view_matrix * model_matrix * vec4(in_position, 1);
    color = in_color;
}
