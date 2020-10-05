// fragment shader
// use with: 'default.vert', 'instance.vert'
#version 330 core

uniform sampler2D tex_sampler;

in vec3 normal;
in vec2 tex;

out vec4 frag_color;

void main() {
    frag_color = texture(tex_sampler, tex);
}