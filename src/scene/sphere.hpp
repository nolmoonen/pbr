#ifndef PBR_SPHERE_HPP
#define PBR_SPHERE_HPP

#include "scene_object.hpp"
#include "scene.hpp"
#include "../system/material.hpp"

class Sphere : public SceneObject {
private:
    Material::MaterialType material;
public:
    Sphere(Scene *scene, Renderer *renderer, glm::vec3 position, Material::MaterialType p_material);

    void render(bool debug_mode) override;

    bool hit(float *t, glm::vec3 origin, glm::vec3 direction) override;
};

#endif //PBR_SPHERE_HPP
