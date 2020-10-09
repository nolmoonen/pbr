#include "nm_math.hpp"

#include <glm/geometric.hpp>

bool nm_math::ray_sphere(float *t, glm::vec3 ray_o, glm::vec3 ray_d, glm::vec3 sphere_o, float sphere_r)
{
    glm::vec3 v = ray_o - sphere_o;
    float discriminant = powf(glm::dot(v, ray_d), 2) - (glm::dot(v, v) - powf(sphere_r, 2));

    if (discriminant < 0) {
        // quantity under the square root is negative, no intersection
        return false;
    }

    float square_root = sqrtf(discriminant);
    float t_neg = -glm::dot(v, ray_d) - square_root;
    float t_pos = -glm::dot(v, ray_d) + square_root;

    if (t_neg < 0 && t_pos < 0) {
        // no positive solution, no intersection
        return false;
    }

    // choose t to be closest intersection point (t >= 0)
    *t = fminf(t_neg, t_pos);

    return true;
}
