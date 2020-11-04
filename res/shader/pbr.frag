// fragment shader
// implementing physically based rendering pipeline (https://learnopengl.com/PBR/Theory)
// requires full primitives
// use with: 'pbr.vert'
#version 330 core

#define NUM_LIGHTS 10
#define M_PI 3.1415926535897932384626433832795

uniform sampler2D texture_diff;     // 0
uniform sampler2D texture_norm;     // 1
uniform sampler2D texture_ao;       // 2
uniform sampler2D texture_rough;    // 3
uniform sampler2D texture_disp;     // 4
uniform sampler2D texture_spec;     // 5

uniform int has_spec;

uniform samplerCube irradiance_map; // 6
uniform samplerCube pre_filter_map; // 7
uniform sampler2D brdf_lut;         // 8

uniform vec3 color_light[NUM_LIGHTS];

in vec2 tex;
in vec3 tangent_pos_light[NUM_LIGHTS];
in vec3 tangent_pos_view;
in vec3 tangent_frag_pos;

out vec4 frag_color;

/*  Fresnel equation:
    Returns the light {f_0} reflected from a surface hit with angle {cos_theta}.
    Uses the Fresnel-Slick approximation. */
vec3 fresnel_schlick(float cos_theta, vec3 f_0);

/*  Fresnel equation with added roughness:
    (https://seblagarde.wordpress.com/2011/08/17/hello-world/) */
vec3 fresnel_schlick_roughness(float cos_theta, vec3 f_0, float roughness);

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

/* Use displacement map to parallax map the texcoords to new ones.
   Uses Parallax Occlusion Mapping.*/
vec2 parallax_mapping(vec2 tex_coords, vec3 view_dir);

void main()
{
    vec3 v          = normalize(tangent_pos_view - tangent_frag_pos); // direction from point to camera
    vec2 tex_coords = parallax_mapping(tex, v);

    // convert SRGB to linear RGB
    vec3 albedo = pow(texture(texture_diff, tex_coords).rgb, vec3(2.2));

    // obtain normal from normal map in range [0, 1], transform to [-1, 1] ({normal} is in tangent space)
    vec3 normal = texture(texture_norm, tex_coords).rgb;
    normal = normalize(normal * 2.0 - 1.0);

    vec3 r = reflect(-v, normal);

    float ao = texture(texture_ao, tex_coords).r;
    float roughness = texture(texture_rough, tex_coords).r;

    float metallic = 0.f;
    if (bool(has_spec)) {
        metallic = texture(texture_spec, tex_coords).r;
    }

    vec3 l_o = vec3(0.0);                  // total outgoing radiance of this fragment
    vec3 f_0 = vec3(0.04);                 // surface reflection at zero incidence (0.04 for dielectric)
    f_0      = mix(f_0, albedo, metallic); // lerp between f_0 and albedo based on metallic value

    for (int i = 0; i < NUM_LIGHTS; i++) {
        vec3 l = normalize(tangent_pos_light[i] - tangent_frag_pos); // direction from point to light
        vec3 h = normalize(v + l);                                   // halfway vector

        // calculate light attenuation
        float distance    = length(tangent_pos_light[i] - tangent_frag_pos); // distance towards light
        float attenuation = 1.0 / (distance * distance); // light attenuation is inversely squarely correlated with dist
        vec3 radiance     = color_light[i] * attenuation;

        // compute approximations
        float ndf = distribution_ggx(normal, h, roughness);
        float g   = geometry_smith(normal, v, l, roughness);
        vec3 f    = fresnel_schlick_roughness(max(dot(h, v), 0.), f_0, roughness);

        vec3 k_s = f;              // specular contribution determined by fresnel approximation
        vec3 k_d = vec3(1.) - k_s; // diffuse contribution by law of energy conservation
        k_d *= 1. - metallic;      // reduce diffuse contribution for metallic surfaces

        // plug in equation and add to outgoing radiance l_o
        vec3 numerator    = ndf * g * f;
        float denominator = 4. * max(dot(normal, v), 0.) * max(dot(normal, l), 0.);
        vec3 specular     = numerator / max(denominator, .001); // prevent div zero

        float n_dot_l = max(dot(normal, l), 0.);
        l_o += (k_d * albedo / M_PI + specular) * radiance * n_dot_l;
    }

    // ambient lighting (we now use IBL as the ambient term)
    vec3 k_s = fresnel_schlick_roughness(max(dot(normal, v), 0.), f_0, roughness);
    vec3 k_d = 1. - k_s;
    k_d *= 1. - metallic;
    vec3 irradiance = texture(irradiance_map, normal).rgb;
    vec3 diffuse    = irradiance * albedo;

    // sample pre-filter map and BRDF lut and combine them together as per the Split-Sum approximation
    const float MAX_REFLECTION_LOD = 4.;
    vec3 prefiltered_color = textureLod(pre_filter_map, r, roughness * MAX_REFLECTION_LOD).rgb;
    vec2 brdf              = texture(brdf_lut, vec2(max(dot(normal, v), 0.), roughness)).rg;
    vec3 specular          = prefiltered_color * (k_s * brdf.x + brdf.y);

    vec3 ambient = (k_d * diffuse + specular) * ao;

    vec3 color = ambient + l_o;

    // reinhard tone mapping + gamma correction
    color = color / (color + vec3(1.));
    color = pow(color, vec3(1. / 2.2));

    frag_color = vec4(color, 1.0);
}

vec3 fresnel_schlick(float cos_theta, vec3 f_0)
{
    return f_0 + (1. - f_0) * pow(1. - cos_theta, 5.);
}

vec3 fresnel_schlick_roughness(float cos_theta, vec3 f_0, float roughness)
{
    return f_0 + (max(vec3(1. - roughness), f_0) - f_0) * pow(1. - cos_theta, 5.);
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

float geometry_schlick_ggx(float n_dot_v, float roughness)
{
    float r = (roughness + 1.);
    float k = (r * r) / 8.;

    float num   = n_dot_v;
    float denom = n_dot_v * (1. - k) + k;

    return num / denom;
}

float geometry_smith(vec3 n, vec3 v, vec3 l, float roughness)
{
    float n_dot_v = max(dot(n, v), 0.);
    float n_dot_l = max(dot(n, l), 0.);
    float ggx2  = geometry_schlick_ggx(n_dot_v, roughness);
    float ggx1  = geometry_schlick_ggx(n_dot_l, roughness);

    return ggx1 * ggx2;
}

vec2 parallax_mapping(vec2 tex_coords, vec3 view_dir)
{
//    return tex_coords;
    const int NUM_LAYERS = 64;
    const float LAYER_DEPTH = 1.f / float(NUM_LAYERS);
    const float HEIGHT_SCALE = .1f; // how accentuated the effects are

    float current_layer_depth = 1.f; // the depth at which the texture is sampled
    vec2 p = HEIGHT_SCALE * view_dir.xy / view_dir.z; // parallax offset vector
    vec2 delta_tex_coords = p / float(NUM_LAYERS);

    vec2  current_tex_coords      = tex_coords; // the tex coords at the current_layer_depth
    float current_depth_map_value = texture(texture_disp, current_tex_coords).r;

    while(current_layer_depth > current_depth_map_value) {
        // shift texture coordinates along direction of p
        current_tex_coords -= delta_tex_coords;
        // get depthmap value at current texture coordinates
        current_depth_map_value = texture(texture_disp, current_tex_coords).r;
        current_layer_depth -= LAYER_DEPTH;
    }

    // get texture coordinates before collision (reverse operations)
    vec2 prev_tex_coords = current_tex_coords + delta_tex_coords;
    float before_depth = texture(texture_disp, prev_tex_coords).r - (current_layer_depth + LAYER_DEPTH);

    // get depth after collision for linear interpolation
    float after_depth  = current_depth_map_value - current_layer_depth;

    // interpolation of texture coordinates
    float weight = after_depth / (after_depth - before_depth);
    vec2 final_tex_coords = prev_tex_coords * weight + current_tex_coords * (1.f - weight);

    return final_tex_coords;
}