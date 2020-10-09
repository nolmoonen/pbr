#include "scene.hpp"

#include "sphere.hpp"

void Scene::render(
        Camera *camera, ShaderManager *shader_manager, TextureManager *texture_manager, MeshManager *mesh_manager,
        bool debug_mode)
{
    for (auto &object : objects) {
        object->render(camera, shader_manager, texture_manager, mesh_manager, debug_mode);
    }

    for (auto &light : lights) {
        light.render(camera, shader_manager, texture_manager, mesh_manager);
    }
}

Scene::Scene()
{
    objects.emplace_back((SceneObject *) new Sphere(this, glm::vec3(0.f)));
    lights.emplace_back(Light(glm::vec3(-1.f, 1.f, -1.f), glm::vec3(1.f, 1.f, 1.f)));
    lights.emplace_back(Light(glm::vec3(-1.f, 1.f, +1.f), glm::vec3(1.f, 1.f, 1.f)));
    lights.emplace_back(Light(glm::vec3(+1.f, 1.f, -1.f), glm::vec3(1.f, 1.f, 1.f)));
    lights.emplace_back(Light(glm::vec3(+1.f, 1.f, +1.f), glm::vec3(1.f, 1.f, 1.f)));
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
