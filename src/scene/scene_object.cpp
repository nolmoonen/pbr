#include "scene_object.hpp"

#include "../system/renderer.hpp"

SceneObject::SceneObject(
        Scene *scene, Renderer *renderer, glm::vec3 position
) :
        scene(scene), renderer(renderer), position(position)
{}

void SceneObject::render(bool debug_mode)
{
    if (debug_mode || selected) {
        renderer->render_lines(
                PRIMITIVE_COORDINATE_SYSTEM,
                glm::translate(glm::identity<glm::mat4>(), position));
    }
}