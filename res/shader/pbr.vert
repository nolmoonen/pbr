// vertex shader
// use with: 'pbr.frag'
#version 330 core

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

uniform vec3 pos_light;
uniform vec3 pos_camera;

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec2 in_texture_coordinates;
layout (location = 2) in vec3 in_normal;
layout (location = 3) in vec3 in_tangent;
layout (location = 4) in vec3 in_bitangent;

out vec2 tex;
out vec3 tangent_pos_light;
out vec3 tangent_pos_view;
out vec3 tangent_frag_pos;

void main()
{
    vec3 frag_pos = vec3(model_matrix * vec4(in_position, 1.0));
    tex = in_texture_coordinates;

    vec3 t = normalize(vec3(model_matrix * vec4(in_tangent,   0.0)));
    vec3 b = normalize(vec3(model_matrix * vec4(in_bitangent, 0.0)));
    vec3 n = normalize(vec3(model_matrix * vec4(in_normal,    0.0)));

    mat3 tbn = transpose(mat3(t, b, n));
    tangent_pos_light = tbn * pos_light;
    tangent_pos_view  = tbn * pos_camera;
    tangent_frag_pos  = tbn * frag_pos;

    gl_Position = projection_matrix * view_matrix * model_matrix * vec4(in_position, 1.0);
}