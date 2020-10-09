#ifndef SCENE_SCENE_HPP
#define SCENE_SCENE_HPP

#include <vector>

#include "light.hpp"
#include "../system/camera.hpp"
#include "../system/shader_manager.hpp"
#include "../system/texture_manager.hpp"
#include "../system/mesh_manager.hpp"
#include "../system/opengl/lines.hpp"

class SceneObject;

class Light;

class Scene {
public:
    std::vector<SceneObject *> objects;
    std::vector<Light *> lights;

    Scene();

    virtual ~Scene();

    void render(
            Camera *camera, ShaderManager *shader_manager, TextureManager *texture_manager, MeshManager *mesh_manager,
            bool debug_mode);

    void update();

    void cast_ray(glm::vec3 origin, glm::vec3 direction);
};

#endif //SCENE_SCENE_HPP
