// fragment shader
// compute brdf lookup table
// use with: 'brdf.vert'
#version 330 core

#define M_PI 3.1415926535897932384626433832795

in vec2 tex_coords;

out vec2 frag_color;

/**
 * Efficient VanDerCorpus calculation.
 * http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html */
float radical_inverse_van_der_corpus(uint bits);

/** */
vec2 hammersley(uint i, uint n);

/** */
vec3 importance_sample_ggx(vec2 x_i, vec3 n, float roughness);

/** NB: clone from {pbr.frag}, with a different k. */
float geometry_schlick_ggx(float n_dot_v, float roughness);

/** NB: clone from {pbr.frag}. */
float geometry_smith(vec3 n, vec3 v, vec3 l, float roughness);

/** */
vec2 integrate_brdf(float n_dot_v, float roughness);

void main()
{
    frag_color = integrate_brdf(tex_coords.x, tex_coords.y);
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

    float phi = 2.0 * M_PI * x_i.x;
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

float geometry_schlick_ggx(float n_dot_v, float roughness)
{
    float a = roughness;
    float k = (a * a) / 2.;

    float numerator   = n_dot_v;
    float denominator = n_dot_v * (1. - k) + k;

    return numerator / denominator;
}

float geometry_smith(vec3 n, vec3 v, vec3 l, float roughness)
{
    float n_dot_v = max(dot(n, v), 0.);
    float n_dot_l = max(dot(n, l), 0.);
    float ggx2    = geometry_schlick_ggx(n_dot_v, roughness);
    float ggx1    = geometry_schlick_ggx(n_dot_l, roughness);

    return ggx1 * ggx2;
}

vec2 integrate_brdf(float n_dot_v, float roughness)
{
    vec3 v = vec3(sqrt(1. - n_dot_v * n_dot_v), 0., n_dot_v);

    float a = 0.;
    float b = 0.;

    vec3 n = vec3(0., 0., 1.);

    const uint SAMPLE_COUNT = 1024u;
    for(uint i = 0u; i < SAMPLE_COUNT; i++) {
        // generates a sample vector biased towards the
        // preferred alignment direction (importance sampling)
        vec2 x_i = hammersley(i, SAMPLE_COUNT);
        vec3 h = importance_sample_ggx(x_i, n, roughness);
        vec3 l = normalize(2. * dot(v, h) * h - v);

        float n_dot_l = max(l.z, 0.);
        float n_dot_h = max(h.z, 0.);
        float v_dot_h = max(dot(v, h), 0.);

        if (n_dot_l > 0.) {
            float g = geometry_smith(n, v, l, roughness);
            float g_vis = (g * v_dot_h) / (n_dot_h * n_dot_v);
            float f_c = pow(1. - v_dot_h, 5.);

            a += (1. - f_c) * g_vis;
            b += f_c * g_vis;
        }
    }

    a /= float(SAMPLE_COUNT);
    b /= float(SAMPLE_COUNT);

    return vec2(a, b);
}