#ifndef SCENE_SCENEOBJECT_HPP
#define SCENE_SCENEOBJECT_HPP

#include "scene.hpp"

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
