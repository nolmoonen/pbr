#include "scene.hpp"

#include "sphere.hpp"

void Scene::render(bool debug_mode)
{
    for (auto &object : objects) {
        object->render(debug_mode);
    }
}

Scene::Scene(
        Renderer *renderer
) :
        renderer(renderer)
{
    objects.emplace_back((SceneObject *) new Sphere(this, renderer, glm::vec3(0.f)));
    lights.emplace_back(new Light(this, renderer, glm::vec3(-1.f, 1.f, -1.f), glm::vec3(1.f, 0.f, 0.f)));
    lights.emplace_back(new Light(this, renderer, glm::vec3(-1.f, 1.f, +1.f), glm::vec3(0.f, 1.f, 0.f)));
    lights.emplace_back(new Light(this, renderer, glm::vec3(+1.f, 1.f, -1.f), glm::vec3(0.f, 0.f, 1.f)));
    lights.emplace_back(new Light(this, renderer, glm::vec3(+1.f, 1.f, +1.f), glm::vec3(1.f, 1.f, 1.f)));

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
    // disable selection for all
    for (auto &object : objects) {
        object->selected = false;
    }

    // find closest intersection
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

    // enable selection for closest, if one is found
    if (closest) closest->selected = true;
}