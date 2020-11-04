// vertex shader
// converting a cubemap texture to a pre-filtered cubemap by sampling it
// use with: 'pre_filter.frag'
#version 330 core

layout (location = 0) in vec3 in_position;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

out vec3 world_pos;

void main()
{
    world_pos = in_position;
    gl_Position = projection_matrix * view_matrix * model_matrix * vec4(world_pos, 1.0);
}