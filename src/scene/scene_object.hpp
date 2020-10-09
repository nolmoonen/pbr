#ifndef SCENE_SCENEOBJECT_HPP
#define SCENE_SCENEOBJECT_HPP

#include <glm/vec3.hpp>
#include "scene.hpp"
#include "../system/camera.hpp"
#include "../system/shader_manager.hpp"
#include "../system/texture_manager.hpp"
#include "../system/mesh_manager.hpp"

class Scene;

class SceneObject {
protected:
    Scene *scene;
    glm::vec3 position;
public:
    SceneObject(Scene *scene, glm::vec3 position);

    virtual ~SceneObject();

    virtual void render(
            Camera *camera, ShaderManager *shader_manager, TextureManager *texture_manager, MeshManager *mesh_manager,
            bool debug_mode) = 0;
};

#endif //SCENE_SCENEOBJECT_HPP
