#ifndef PBR_SPHERE_HPP
#define PBR_SPHERE_HPP

#include "scene_object.hpp"
#include "../system/opengl/lines.hpp"
#include "scene.hpp"

class Sphere : public SceneObject {
private:
    Lines normals{}; // todo debug
public:
    Sphere(Scene *scene, Renderer *renderer, glm::vec3 position);

    void render(bool debug_mode) override;

    bool hit(float *t, glm::vec3 origin, glm::vec3 direction) override;
};

#endif //PBR_SPHERE_HPP
