#include "sphere.hpp"

#include "../util/nm_math.hpp"
#include "../system/material.hpp"
#include "../system/renderer.hpp"

Sphere::Sphere(
        Scene *scene, Renderer *renderer, glm::vec3 position
) :
        SceneObject(scene, renderer, position)
{
    Lines::create_sphere(&normals); // todo debug
}

void Sphere::render(bool debug_mode)
{
    if (debug_mode) {
        renderer->render_lines(&normals, glm::identity<glm::mat4>());
    }

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> colors;
    for (auto &light : scene->lights) {
        positions.emplace_back(light->position);
        colors.emplace_back(light->color);
    }

    renderer->render_pbr(PRIMITIVE_SPHERE, Material::MATERIAL_BRICK_2K, positions, colors, glm::identity<glm::mat4>());
}

bool Sphere::hit(float *t, glm::vec3 origin, glm::vec3 direction)
{
    return nm_math::ray_sphere(t, origin, direction, position, 1.f);
}
