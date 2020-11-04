#ifndef SYSTEM_EMBEDDED_HPP
#define SYSTEM_EMBEDDED_HPP

#include <cstdlib>

/**
 * Specified manually based on identifiers in CMakeLists.
 */

/** Shader */
extern const char default_vert[];
extern const size_t default_vert_len;

extern const char default_frag[];
extern const size_t default_frag_len;

extern const char phong_vert[];
extern const size_t phong_vert_len;

extern const char phong_frag[];
extern const size_t phong_frag_len;

extern const char lines_vert[];
extern const size_t lines_vert_len;

extern const char lines_frag[];
extern const size_t lines_frag_len;

extern const char pbr_vert[];
extern const size_t pbr_vert_len;

extern const char pbr_frag[];
extern const size_t pbr_frag_len;

extern const char equirectangular_map_vert[];
extern const size_t equirectangular_map_vert_len;

extern const char equirectangular_map_frag[];
extern const size_t equirectangular_map_frag_len;

extern const char skybox_vert[];
extern const size_t skybox_vert_len;

extern const char skybox_frag[];
extern const size_t skybox_frag_len;

extern const char irradiance_map_vert[];
extern const size_t irradiance_map_vert_len;

extern const char irradiance_map_frag[];
extern const size_t irradiance_map_frag_len;

/** Texture */

extern const char test_png[];
extern const size_t test_png_len;

extern const char brick_diff_png[];
extern const size_t brick_diff_png_len;

extern const char brick_norm_png[];
extern const size_t brick_norm_png_len;

extern const char brick_ao_png[];
extern const size_t brick_ao_png_len;

extern const char brick_rough_png[];
extern const size_t brick_rough_png_len;

extern const char brick_disp_png[];
extern const size_t brick_disp_png_len;

extern const char cayley_interior_hdr[];
extern const size_t cayley_interior_hdr_len;

extern const char studio_hdr[];
extern const size_t studio_hdr_len;

#endif //SYSTEM_EMBEDDED_HPP
