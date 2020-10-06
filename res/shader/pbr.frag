// fragment shader
// use with: 'pbr.vert'
#version 330 core

uniform sampler2D tex_sampler;

uniform vec3 pos_light;
uniform vec3 pos_camera;
uniform vec3 color_light;

uniform vec3  albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;

out vec3 geom;
in vec2 tex;
in vec3 normal;

out vec4 frag_color;

#define M_PI 3.1415926535897932384626433832795

/*  Fresnel equation:
    Returns the light {f_0} reflected from a surface hit with angle {cos_theta}.
    Uses the Fresnel-Slick approximation. */
vec3 fresnel_schlick(float cos_theta, vec3 f_0);

/* Normal distribution function:
   Given surface normal {n} and halfway vector {h} and {roughness}, approximate the relative surface area aligned
   to the halfway vector.
   Uses Trowbridge-Reitz GGX normal distribution function. */
float distribution_ggx(vec3 n, vec3 h, float roughness);

/* Schlick-GGX (GGX + Schlick-Beckmann) */
float geometry_schlick_ggx(float n_dot_v, float roughness);

/* Geometry function:
   Given surface normal {n}, view direction {v}, light direction {l}, and {roughness}, approximate the relative
   surface area where micro surface details overshadow each other, obstructing light rays.
   Uses Smith's method (which uses Schlick-GGX). */
float geometry_smith(vec3 n, vec3 v, vec3 l, float roughness);

void main()
{
    vec3 v   = normalize(pos_camera - geom); // direction from point to camera
    vec3 l_o = vec3(0.0);                    // total outgoing radiance of this fragment
    vec3 f_0 = vec3(0.04);                   // surface reflection at zero incidence (0.04 for dielectric)
    f_0      = mix(f_0, albedo, metallic);   // lerp between f_0 and albedo based on metallic value

    for (int i = 0; i < 1; i++) {
        vec3 l = normalize(pos_light - geom); // direction from point to light
        vec3 h = normalize(v + l);            // halfway vector

        // calculate light attenuation
        float distance    = length(pos_light - geom);    // distance towards light
        float attenuation = 1.0 / (distance * distance); // light attenuation is inversely squarely correlated with dist
        vec3 radiance     = color_light * attenuation;

        // compute approximations
        float ndf = distribution_ggx(normal, h, roughness);
        float g   = geometry_smith(normal, v, l, roughness);
        vec3 f   = fresnel_schlick(max(dot(h, v), 0.0), f_0);

        vec3 k_s = f;               // specular contribution determined by fresnel approximation
        vec3 k_d = vec3(1.0) - k_s; // diffuse contribution by law of energy conservation
        k_d *= 1.0 - metallic;      // reduce diffuse contribution for metallic surfaces

        // plug in equation and add to outgoing radiance l_o
        vec3 numerator    = ndf * g * f;
        float denominator = 4.0 * max(dot(normal, v), 0.0) * max(dot(normal, l), 0.0);
        vec3 specular     = numerator / max(denominator, 0.001);

        float n_dot_l = max(dot(normal, l), 0.0);
        l_o += (k_d * albedo / M_PI + specular) * radiance * n_dot_l;
    }

    vec3 ambient = vec3(0.03) * albedo * ao; // improvise ambient intensity
    vec3 color = ambient + l_o;

    // reinhard tone mapping
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    frag_color = vec4(color, 1.0);
}

vec3 fresnel_schlick(float cos_theta, vec3 f_0)
{
    return f_0 + (1.0 - f_0) * pow(1.0 - cos_theta, 5.0);
}

float distribution_ggx(vec3 n, vec3 h, float roughness)
{
    float a         = roughness * roughness;
    float a2        = a * a;
    float n_dot_h   = max(dot(n, h), 0.0);
    float n_dot_h_2 = n_dot_h * n_dot_h;

    float num   = a2;
    float denom = (n_dot_h_2 * (a2 - 1.0) + 1.0);
    denom = M_PI * denom * denom;

    return num / denom;
}

float geometry_schlick_ggx(float n_dot_v, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num   = n_dot_v;
    float denom = n_dot_v * (1.0 - k) + k;

    return num / denom;
}

float geometry_smith(vec3 n, vec3 v, vec3 l, float roughness)
{
    float n_dot_v = max(dot(n, v), 0.0);
    float n_dot_l = max(dot(n, l), 0.0);
    float ggx2  = geometry_schlick_ggx(n_dot_v, roughness);
    float ggx1  = geometry_schlick_ggx(n_dot_l, roughness);

    return ggx1 * ggx2;
}