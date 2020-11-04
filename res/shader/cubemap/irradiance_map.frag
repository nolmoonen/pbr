// fragment shader
// converting a cubemap texture to an irradiance cubemap by sampling it
// use with: 'irradiance_map.vert'
#version 330 core

#define M_PI 3.1415926535897932384626433832795

uniform samplerCube environment_map;

in vec3 world_pos;

out vec4 frag_color;

void main()
{
    // the world vector acts as the normal of a tangent surface from the origin, aligned to {world_pos}. Given this
    // normal, calculate all incoming radiance of the environment. The result of this radiance is the radiance of light
    // coming from {-n} direction, which is what we use in the PBR shader to sample irradiance.
    vec3 n = normalize(world_pos);

    vec3 irradiance = vec3(0.);

    // tangent space calculation from origin point
    vec3 up    = vec3(0., 1., 0.);
    vec3 right = cross(up, n);
    up         = cross(n, right);

    const float SECTOR_COUNT = 250;
    const float STACK_COUNT = 60;

    const float SECTOR_STEP = (2. * M_PI) / SECTOR_COUNT;
    const float STACK_STEP = (.5 * M_PI) / STACK_COUNT;

    for (uint x = 0u; x < SECTOR_COUNT; x++) {
        float phi = SECTOR_STEP * x;
        for (uint y = 0u; y < STACK_COUNT; y++) {
            float theta = STACK_STEP * y;

            // spherical to cartesian (in tangent space)
            vec3 tangent_sample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
            // tangent space to world
            vec3 sample_vec = tangent_sample.x * right + tangent_sample.y * up + tangent_sample.z * n;

            irradiance += texture(environment_map, sample_vec).rgb * cos(theta) * sin(theta);
        }
    }

    irradiance = M_PI * irradiance * (1. / float(SECTOR_COUNT * STACK_COUNT));

    frag_color = vec4(irradiance, 1.);
}