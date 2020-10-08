#ifndef SCENE_SCENE_HPP
#define SCENE_SCENE_HPP

#include <vector>
#include <cstdint>

#include "../system/texture_manager.hpp"
#include "../system/mesh_manager.hpp"
#include "../system/shader_manager.hpp"
#include "../system/camera.hpp"
#include "light.hpp"
#include "scene_object.hpp"

class SceneObject;

class Scene {
public:
    std::vector<SceneObject *> objects;
    std::vector<Light> lights;

    Scene();

    virtual ~Scene();

    void render(
            Camera *camera, ShaderManager *shader_manager, TextureManager *texture_manager, MeshManager *mesh_manager,
            bool debug_mode);

    void update();
};

#endif //SCENE_SCENE_HPP
