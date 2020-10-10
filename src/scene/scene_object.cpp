#include "scene_object.hpp"

SceneObject::SceneObject(Scene *scene, glm::vec3 position) : scene(scene), position(position)
{}

SceneObject::~SceneObject()
{}

void SceneObject::render(
        Camera *camera, ShaderManager *shader_manager, TextureManager *texture_manager,
        MeshManager *mesh_manager, bool debug_mode)
{
//    Renderer::render_coordinate(glm::translate(glm::identity<glm::mat4>(), position));
}