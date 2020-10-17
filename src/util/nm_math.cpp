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
    *t = fminf(t_neg, t_pos); // todo should this reject negative t?

    return true;
}

bool nm_math::ray_infinite_cylinder(
        float *t1, bool *t1_hit, float *t2, bool *t2_hit,
        glm::vec3 ray_o, glm::vec3 ray_d, glm::vec3 cyl_pa, glm::vec3 cyl_va, float cyl_r)
{
    // https://mrl.nyu.edu/~dzorin/rend05/lecture2.pdf
    glm::vec3 tmp = ray_d - glm::dot(ray_d, cyl_va) * cyl_va;
    float a = glm::dot(tmp, tmp);
    glm::vec3 dp = ray_o - cyl_pa;
    glm::vec3 tmp1 = dp - glm::dot(dp, cyl_va) * cyl_va;
    float b = 2.f * glm::dot(tmp, tmp1);
    float c = glm::dot(tmp1, tmp1) - cyl_r * cyl_r;
    float det = b * b - 4.f * a * c;

    if (det < 0) {
        *t1_hit = false;
        *t2_hit = false;
        return false;
    } else if (det == 0) {
        *t1 = -b / (2.f * a);
        *t1_hit = true;
        *t2_hit = false;
        return (*t1 > 0);
    } else { // det > 0
        *t1_hit = true;
        *t2_hit = true;
        *t1 = (-b - sqrtf(det)) / (2.f * a);
        *t2 = (-b + sqrtf(det)) / (2.f * a);
        return (*t1 > 0 || *t2 > 0);
    }
}

bool nm_math::ray_plane(float *t, glm::vec3 ray_o, glm::vec3 ray_d, glm::vec3 plane_p, glm::vec3 plane_n)
{
    float denom = glm::dot(plane_n, ray_d);
    if (fabsf(denom) > 0.f) {
        *t = glm::dot(plane_p - ray_o, plane_n) / denom;
        return *t >= 0;
    }

    return false;
}

bool nm_math::ray_cylinder(float *t, glm::vec3 ray_o, glm::vec3 ray_d, glm::vec3 cyl_p1, glm::vec3 cyl_p2, float cyl_r)
{
    // https://mrl.nyu.edu/~dzorin/rend05/lecture2.pdf
    glm::vec3 va = glm::normalize(cyl_p2 - cyl_p1);
    float t1, t2, t3, t4;                        // intersection points of infinite cylinder and plane
    bool t1_hit, t2_hit, t3_hit, t4_hit;         // whether ray hits infinite cylinder and plane
    bool t1_valid, t2_valid, t3_valid, t4_valid; // whether ti is a valid, >= 0 and intersection on correct place

    // test ray-infinite cylinder intersection
    ray_infinite_cylinder(&t1, &t1_hit, &t2, &t2_hit, ray_o, ray_d, cyl_p1, va, cyl_r);

    // intersection is valid if >= 0 and hit the bounded part of the cylinder
    glm::vec3 q1 = ray_o + t1 * ray_d;
    t1_valid = t1_hit && t1 >= 0.f && glm::dot(va, q1 - cyl_p1) > 0.f && glm::dot(va, q1 - cyl_p2) < 0.f;
    glm::vec3 q2 = ray_o + t2 * ray_d;
    t2_valid = t2_hit && t2 >= 0.f && glm::dot(va, q2 - cyl_p1) > 0.f && glm::dot(va, q2 - cyl_p2) < 0.f;

    // test both planes which bound the infinite cylinder
    t3_hit = ray_plane(&t3, ray_o, ray_d, cyl_p1, va); // bottom cap
    t4_hit = ray_plane(&t4, ray_o, ray_d, cyl_p2, va); // top cap

    // intersection is valid if >= 0 and hit the bounded part of the plane
    glm::vec3 tmp3 = (ray_o + t3 * ray_d) - cyl_p1;
    t3_valid = t3_hit && t3 >= 0.f && (glm::dot(tmp3, tmp3) < cyl_r * cyl_r);
    glm::vec3 tmp4 = (ray_o + t4 * ray_d) - cyl_p2;
    t4_valid = t4_hit && t4 >= 0.f && (glm::dot(tmp4, tmp4) < cyl_r * cyl_r);

    // if none are valid, return false
    if (!(t1_valid || t2_valid || t3_valid || t4_valid)) return false;

    // else, find the minimum t out of all valid ones
    *t = std::numeric_limits<float>::max();
    if (t1_valid) *t = fminf(*t, t1);
    if (t2_valid) *t = fminf(*t, t2);
    if (t3_valid) *t = fminf(*t, t3);
    if (t4_valid) *t = fminf(*t, t4);

    return true;
}

bool nm_math::ray_cone(
        float *t, glm::vec3 ray_o, glm::vec3 ray_d, glm::vec3 cone_c, glm::vec3 cone_v,
        float cone_height, float cone_radius)
{
    // https://lousodrome.net/blog/light/2017/01/03/intersection-of-a-ray-and-a-cone/#:~:text=We%20define%20a%20ray%20with,O%20%E2%86%92%20%2B%20t%20D%20%E2%86%92%20.
    float cone_theta = atanf(cone_radius / cone_height);
    float d_dot_v = glm::dot(ray_d, cone_v);
    float cos_theta_2 = cosf(cone_theta) * cosf(cone_theta);
    float a = d_dot_v * d_dot_v - cos_theta_2;
    glm::vec3 co = ray_o - cone_c;
    float co_dot_v = glm::dot(co, cone_v);
    float b = 2.f * (d_dot_v * co_dot_v - glm::dot(ray_d, co) * cos_theta_2);
    float c = co_dot_v * co_dot_v - glm::dot(co, co) * cos_theta_2;
    float det = b * b - 4.f * a * c;

    // find smallest t >= 0
    if (det < 0) {
        return false;
    } else if (det == 0) {
        *t = -b / (2.f * a);
        if (*t < 0) return false;
    } else { // det > 0
        float t1 = (-b - sqrtf(det)) / (2.f * a);
        float t2 = (-b + sqrtf(det)) / (2.f * a);
        if (t1 < 0 && t2 < 0) {
            return false;
        } else if (t1 < 0 && t2 > 0) {
            *t = t2;
        } else if (t1 > 0 && t2 < 0) {
            *t = t1;
        } else { // (t1 > 0 && t2 > 0)
            *t = fminf(t1, t2);
        }
    }

    glm::vec3 p = ray_o + *t * ray_d;
    // reject if shadow cone
    glm::vec3 cp = p - cone_c;
    if (cone_theta < 90.f && glm::dot(cp, cone_v) <= 0) return false;
    // reject if p lies beyond base
    if (glm::length(cp) > sqrtf(cone_height * cone_height + cone_radius * cone_radius)) return false;

    return true;
}