// vertex shader
// converting an equirectangular texture to a cubemap by rendering it onto a unit cube
// use with: 'equirectangular_map.frag'
#version 330 core

layout (location = 0) in vec3 in_position;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

out vec3 local_pos;

void main()
{
    local_pos = in_position;
    gl_Position = projection_matrix * view_matrix * model_matrix * vec4(local_pos, 1.0);
}