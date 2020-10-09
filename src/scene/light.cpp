#include "light.hpp"
#include "../util/nm_math.hpp"

Light::Light(
        Scene *scene, glm::vec3 position, glm::vec3 color
) :
        SceneObject(scene, position), color(color)
{}

void Light::render(
        Camera *camera, ShaderManager *shader_manager, TextureManager *texture_manager, MeshManager *mesh_manager,
        bool debug_mode)
{
    ShaderProgram *program = shader_manager->get(SHADER_DEFAULT);
    ShaderProgram::use_shader_program(program);
    ShaderProgram::set_mat4(program, "model_matrix",
                            glm::scale(glm::translate(
                                    glm::identity<glm::mat4>(),
                                    position), glm::vec3(1.f / SCALE)));
    ShaderProgram::set_mat4(program, "view_matrix", camera->get_view_matrix());
    ShaderProgram::set_mat4(program, "projection_matrix", camera->get_proj_matrix());

    ShaderProgram::set_vec3(program, "pos_camera", camera->get_camera_position());

    Mesh::render_mesh(mesh_manager->get(MESH_SPHERE));

    ShaderProgram::unuse_shader_program();
}

bool Light::hit(float *t, glm::vec3 origin, glm::vec3 direction)
{
    return nm_math::ray_sphere(t, origin, direction, position, 1.f / SCALE);
}