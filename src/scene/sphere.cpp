#include "sphere.hpp"

#include "../util/nm_math.hpp"
#include "../system/material.hpp"
#include "../system/renderer.hpp"

Sphere::Sphere(
        Scene *scene, Renderer *renderer, glm::vec3 position
) :
        SceneObject(scene, renderer, position)
{}

void Sphere::render(bool debug_mode)
{
    SceneObject::render(debug_mode);

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> colors;
    for (auto &light : scene->lights) {
        positions.emplace_back(light->position);
        colors.emplace_back(light->color);
    }

    renderer->render_pbr(
            PRIMITIVE_SPHERE, Material::MATERIAL_BRICK_1K, positions, colors,
            glm::translate(glm::identity<glm::mat4>(), position));
}

bool Sphere::hit(float *t, glm::vec3 origin, glm::vec3 direction)
{
    return nm_math::ray_sphere(t, origin, direction, position, 1.f);
}
