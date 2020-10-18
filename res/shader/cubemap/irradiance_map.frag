// fragment shader
// converting a cubemap texture to an irradiance cubemap by sampling it
// use with: 'irradiance_map.vert'
#version 330 core

uniform samplerCube environment_map;

in vec3 world_pos;

out vec4 frag_color;

void main()
{
    const float PI = 3.14159265359;
    // The world vector acts as the normal of a tangent surface from the origin, aligned to {world_pos}. Given this
    // normal, calculate all incoming radiance of the environment. The result of this radiance is the radiance of light
    // coming from {-n} direction, which is what we use in the PBR shader to sample irradiance.
    vec3 n = normalize(world_pos);

    vec3 irradiance = vec3(0.0);

    // tangent space calculation from origin point
    vec3 up    = vec3(0.0, 1.0, 0.0);
    vec3 right = cross(up, n);
    up         = cross(n, right);

    float sample_delta = 0.025;
    float sample_count = 0.0;
    for (float phi = 0.0; phi < 2.0 * PI; phi += sample_delta) {
        for (float theta = 0.0; theta < 0.5 * PI; theta += sample_delta) {
            // spherical to cartesian (in tangent space)
            vec3 tangent_sample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
            // tangent space to world
            vec3 sample_vec = tangent_sample.x * right + tangent_sample.y * up + tangent_sample.z * n;

            irradiance += texture(environment_map, sample_vec).rgb * cos(theta) * sin(theta);
            sample_count++;
        }
    }
    irradiance = PI * irradiance * (1.0 / float(sample_count));

    frag_color = vec4(irradiance, 1.0);
}