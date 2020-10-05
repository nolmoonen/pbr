// fragment shader
// use with: 'phong.vert'
#version 330 core

uniform sampler2D tex_sampler;

uniform vec3 pos_light;
uniform vec3 pos_camera;
uniform vec3 color_light;

in vec3 normal;
in vec3 pos;
in vec2 tex;

out vec4 frag_color;

void main()
{
    // ambient
    float ambient_strength = .3f;
    vec3 ambient = ambient_strength * color_light;

    // diffuse
    vec3 norm = normalize(normal);
    vec3 dir_light = normalize(pos_light - pos);
    float diff = max(dot(norm, dir_light), 0.f);
    vec3 diffuse = diff * color_light;

    // specular
    float strength_specular = .5f;
    vec3 dir_view = normalize(pos_camera - pos);
    vec3 dir_reflect = reflect(-dir_light, norm);
    float spec = pow(max(dot(dir_view, dir_reflect), .0f), 32);
    vec3 specular = strength_specular * spec * color_light;

    vec3 color_object = texture(tex_sampler, tex).xyz;
    vec3 result = (ambient + diffuse + specular) * color_object;
    frag_color = vec4(result, 1.f);
}