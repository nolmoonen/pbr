#ifndef UTIL_NM_MATH_HPP
#define UTIL_NM_MATH_HPP

#include <glm/vec3.hpp>

namespace nm_math {
    /**
     * {ray_d} should be unitized.
     * If returns true, {t} is positive. */
    bool ray_sphere(float *t, glm::vec3 ray_o, glm::vec3 ray_d, glm::vec3 sphere_o, float sphere_r);
}

#endif //UTIL_NM_MATH_HPP
