// vertex shader
// render a cubemapped skybox
// use with: 'skybox.frag'
#version 330 core

layout (location = 0) in vec3 in_position;

uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform mat4 model_matrix;

out vec3 local_pos;

void main()
{
    local_pos = in_position;

    mat4 rot_view = mat4(mat3(view_matrix)); // remove translation from the view matrix
    vec4 clip_pos = projection_matrix * rot_view * model_matrix * vec4(local_pos, 1.0);

    gl_Position = clip_pos.xyww;
}