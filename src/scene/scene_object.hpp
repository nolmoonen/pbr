#ifndef SCENE_SCENEOBJECT_HPP
#define SCENE_SCENEOBJECT_HPP

#include <glm/vec3.hpp>

class Scene;

class Renderer;

class SceneObject {
protected:
    Scene *scene;
    Renderer *renderer;
public:
    glm::vec3 position;

    bool selected = false;

    SceneObject(Scene *scene, Renderer *renderer, glm::vec3 position);

    virtual ~SceneObject() = default;

    virtual void render(bool debug_mode);

    virtual bool hit(float *t, glm::vec3 origin, glm::vec3 direction) = 0;
};

#endif //SCENE_SCENEOBJECT_HPP
