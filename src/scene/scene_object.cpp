#include "scene_object.hpp"

SceneObject::SceneObject(
        Scene *scene, Renderer *renderer, glm::vec3 position
) :
        scene(scene), renderer(renderer), position(position)
{}

SceneObject::~SceneObject()
{}

void SceneObject::render(bool debug_mode)
{
//    Renderer::render_coordinate(glm::translate(glm::identity<glm::mat4>(), position));
}