// fragment shader
// converting a cubemap texture to a pre-filtered cubemap by sampling it
// use with: 'pre_filter.vert'
#version 330 core

#define M_PI 3.1415926535897932384626433832795

uniform samplerCube environment_map;
uniform float roughness;

in vec3 world_pos;

out vec4 frag_color;

/** NB: clone from {pbr.frag}. */
float distribution_ggx(vec3 n, vec3 h, float roughness);

/** NB: clone from {brdf.frag}. */
float radical_inverse_van_der_corpus(uint bits);

/** NB: clone from {brdf.frag}. */
vec2 hammersley(uint i, uint n);

/** NB: clone from {brdf.frag}. */
vec3 importance_sample_ggx(vec2 x_i, vec3 n, float roughness);

void main()
{
    vec3 n = normalize(world_pos);

    // make the simplyfying assumption that V equals R equals the normal
    vec3 r = n;
    vec3 v = r;

    const uint SAMPLE_COUNT = 1024u;
    vec3 pre_filtered_color = vec3(0.);
    float total_weight = 0.;

    for (uint i = 0u; i < SAMPLE_COUNT; ++i) {
        // generates a sample vector that's biased towards the preferred alignment direction (importance sampling).
        vec2 x_i = hammersley(i, SAMPLE_COUNT);
        vec3 h = importance_sample_ggx(x_i, n, roughness);
        vec3 l  = normalize(2. * dot(v, h) * h - v);

        float n_dot_l = max(dot(n, l), 0.);
        if (n_dot_l > 0.) {
            // sample from the environment's mip level based on roughness/pdf
            float d       = distribution_ggx(n, h, roughness);
            float n_dot_h = max(dot(n, h), 0.);
            float h_dot_v = max(dot(h, v), 0.);
            float pdf     = d * n_dot_h / (4. * h_dot_v) + .0001;

            float resolution = 512.; // resolution of source cubemap (per face)
            float sa_texel   = 4. * M_PI / (6. * resolution * resolution);
            float sa_sample  = 1. / (float(SAMPLE_COUNT) * pdf + .0001);

            float mip_level = roughness == 0. ? 0. : .5 * log2(sa_sample / sa_texel);

            pre_filtered_color += textureLod(environment_map, l, mip_level).rgb * n_dot_l;
            total_weight       += n_dot_l;
        }
    }

    pre_filtered_color = pre_filtered_color / total_weight;

    frag_color = vec4(pre_filtered_color, 1.);
}

float distribution_ggx(vec3 n, vec3 h, float roughness)
{
    float a         = roughness * roughness;
    float a2        = a * a;
    float n_dot_h   = max(dot(n, h), 0.);
    float n_dot_h_2 = n_dot_h * n_dot_h;

    float numerator   = a2;
    float denominator = (n_dot_h_2 * (a2 - 1.) + 1.);
    denominator       = M_PI * denominator * denominator;

    return numerator / denominator;
}

float radical_inverse_van_der_corpus(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 hammersley(uint i, uint n)
{
    return vec2(float(i) / float(n), radical_inverse_van_der_corpus(i));
}

vec3 importance_sample_ggx(vec2 x_i, vec3 n, float roughness)
{
    float a = roughness * roughness;

    float phi = 2. * M_PI * x_i.x;
    float cos_theta = sqrt((1. - x_i.y) / (1. + (a * a - 1.) * x_i.y));
    float sin_theta = sqrt(1. - cos_theta * cos_theta);

    // from spherical coordinates to cartesian coordinates - halfway vector
    vec3 h = vec3(cos(phi) * sin_theta, sin(phi) * sin_theta, cos_theta);

    // from tangent-space H vector to world-space sample vector
    vec3 up        = abs(n.z) < .999 ? vec3(0., 0., 1.) : vec3(1., 0., 0.);
    vec3 tangent   = normalize(cross(up, n));
    vec3 bitangent = cross(n, tangent);

    vec3 sample_vec = tangent * h.x + bitangent * h.y + n * h.z;
    return normalize(sample_vec);
}