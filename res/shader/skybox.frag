// fragment shader
// render a cubemapped skybox
// use with: 'skybox.vert'
#version 330 core

uniform samplerCube environment_map;

in vec3 local_pos;

out vec4 frac_color;

void main()
{
    vec3 env_color = texture(environment_map, local_pos).rgb;

    // environment map is in hdr, so apply reinhard to tone map to ldr
    env_color = env_color / (env_color + vec3(1.0));
    env_color = pow(env_color, vec3(1.0 / 2.2));

    frac_color = vec4(env_color, 1.0);
}