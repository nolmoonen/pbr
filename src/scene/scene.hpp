#ifndef SCENE_SCENE_HPP
#define SCENE_SCENE_HPP

#include <vector>

#include "light.hpp"
#include "../system/camera.hpp"
#include "../system/manager/shader_manager.hpp"
#include "../system/manager/texture_manager.hpp"
#include "../system/manager/primitive_manager.hpp"
#include "../system/opengl/lines.hpp"

class SceneObject;

class Light;

class Scene {
private:
    Renderer *renderer;
public:
    std::vector<SceneObject *> objects;
    std::vector<Light *> lights;

    explicit Scene(Renderer *renderer);

    virtual ~Scene();

    void render(bool debug_mode);

    void update();

    void cast_ray(glm::vec3 origin, glm::vec3 direction);
};

#endif //SCENE_SCENE_HPP
