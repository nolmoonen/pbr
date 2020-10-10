#ifndef PBR_LIGHT_HPP
#define PBR_LIGHT_HPP

#include <glm/vec3.hpp>

#include "scene_object.hpp"

class Light : public SceneObject {
private:
    const float SCALE = 10.f;
public:
    glm::vec3 color;

    Light(Scene *scene, Renderer *renderer, glm::vec3 position, glm::vec3 color);

    void render(bool debug_mode) override;

    bool hit(float *t, glm::vec3 origin, glm::vec3 direction) override;
};

#endif //PBR_LIGHT_HPP
