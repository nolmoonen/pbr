#ifndef UTIL_NM_MATH_HPP
#define UTIL_NM_MATH_HPP

#include <glm/vec3.hpp>

namespace nm_math {
    /** {ray_d} should be unitized. If true is returned, {t} is >= 0. */
    bool ray_sphere(float *t, glm::vec3 ray_o, glm::vec3 ray_d, glm::vec3 sphere_o, float sphere_r);

    /** {ray_d} and {cyl_va} should be unitized. If true is returned, {t1} or {t2} is >= 0. */
    bool ray_infinite_cylinder(
            float *t1, bool *t1_hit, float *t2, bool *t2_hit,
            glm::vec3 ray_o, glm::vec3 ray_d, glm::vec3 cyl_pa, glm::vec3 cyl_va, float cyl_r);

    /** {ray_d} and {plane_n} should be unitized. If true is returned, {t} is >= 0. */
    bool ray_plane(
            float *t, glm::vec3 ray_o, glm::vec3 ray_d, glm::vec3 plane_p, glm::vec3 plane_n);

    /** {ray_d} should be unitized. If true is returned, {t} is >= 0. */
    bool ray_cylinder(float *t, glm::vec3 ray_o, glm::vec3 ray_d, glm::vec3 cyl_p1, glm::vec3 cyl_p2, float cyl_r);

    /** {ray_d} and {cone_v} should be unitized. {cone_theta} in degrees. If true is returned, {t} is >= 0. */
    bool ray_cone(
            float *t, glm::vec3 ray_o, glm::vec3 ray_d, glm::vec3 cone_c, glm::vec3 cone_v,
            float cone_height, float cone_radius);
}

#endif //UTIL_NM_MATH_HPP
