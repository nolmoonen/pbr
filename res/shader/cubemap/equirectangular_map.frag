// fragment shader
// converting an equirectangular texture to a cubemap by rendering it onto a unit cube
// use with: 'equirectangular_map.vert'
#version 330 core

uniform sampler2D equirectangular_map;

in vec3 local_pos;

out vec4 frag_color;

vec2 sample_spherical_map(vec3 v)
{
    const vec2 inv_atan = vec2(0.1591, 0.3183);
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= inv_atan;
    uv += 0.5;
    return uv;
}

void main()
{
    vec2 uv = sample_spherical_map(normalize(local_pos)); // make sure to normalize local_pos
    vec3 color = texture(equirectangular_map, uv).rgb;

    frag_color = vec4(color, 1.0);
}
