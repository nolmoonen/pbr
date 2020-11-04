// vertex shader
// compute brdf lookup table
// use with: 'brdf.frag'
#version 330 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_tex;

out vec2 tex_coords;

void main()
{
    tex_coords = in_tex;
    gl_Position = vec4(in_pos, 1.0);
}