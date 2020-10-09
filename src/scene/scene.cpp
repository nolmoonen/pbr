#include "scene.hpp"

#include "sphere.hpp"

void Scene::render(
        Camera *camera, ShaderManager *shader_manager, TextureManager *texture_manager, MeshManager *mesh_manager,
        bool debug_mode)
{
    for (auto &object : objects) {
        object->render(camera, shader_manager, texture_manager, mesh_manager, debug_mode);
    }
}

Scene::Scene()
{
    objects.emplace_back((SceneObject *) new Sphere(this, glm::vec3(0.f)));
    lights.emplace_back(new Light(this, glm::vec3(-1.f, 1.f, -1.f), glm::vec3(1.f, 1.f, 1.f)));
    lights.emplace_back(new Light(this, glm::vec3(-1.f, 1.f, +1.f), glm::vec3(1.f, 1.f, 1.f)));
    lights.emplace_back(new Light(this, glm::vec3(+1.f, 1.f, -1.f), glm::vec3(1.f, 1.f, 1.f)));
    lights.emplace_back(new Light(this, glm::vec3(+1.f, 1.f, +1.f), glm::vec3(1.f, 1.f, 1.f)));

    // little bit awkward, but having Light a child of SceneObject allows for nice code elsewhere
    for (auto &light : lights) {
        objects.emplace_back(light);
    }
}

void Scene::update()
{
    // todo?
}

Scene::~Scene()
{
    for (auto &object : objects) {
        delete object;
    }
}

void Scene::cast_ray(glm::vec3 origin, glm::vec3 direction)
{
    SceneObject *closest = nullptr;
    float closest_t = std::numeric_limits<float>::max();
    for (auto &object : objects) {
        float t;
        bool hit = object->hit(&t, origin, direction);
        if (hit && t < closest_t) {
            closest_t = t;
            closest = object;
        }
    }

    if (closest) closest->selected = true;
}