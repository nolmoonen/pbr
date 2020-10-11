#include "light.hpp"

#include "../util/nm_math.hpp"
#include "../system/renderer.hpp"

Light::Light(
        Scene *scene, Renderer *renderer, glm::vec3 position, glm::vec3 color
) :
        SceneObject(scene, renderer, position), color(color)
{}

void Light::render(bool debug_mode)
{
    renderer->render_default(
            PRIMITIVE_SPHERE,
            color,
            glm::scale(
                    glm::translate(
                            glm::identity<glm::mat4>(),
                            position),
                    glm::vec3(1.f / SCALE)));
}

bool Light::hit(float *t, glm::vec3 origin, glm::vec3 direction)
{
    return nm_math::ray_sphere(t, origin, direction, position, 1.f / SCALE);
}