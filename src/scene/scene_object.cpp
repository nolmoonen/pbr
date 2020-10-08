#include "scene_object.hpp"

SceneObject::SceneObject(Scene *scene, glm::vec3 position) : scene(scene), position(position)
{}

SceneObject::~SceneObject()
{

}