#include "light.hpp"

Light::Light(glm::vec3 position, glm::vec3 color) : position(position), color(color)
{}

void Light::render(
        Camera *camera, ShaderManager *shader_manager, TextureManager *texture_manager, MeshManager *mesh_manager) const
{
    ShaderProgram *program = shader_manager->get(SHADER_DEFAULT);
    ShaderProgram::use_shader_program(program);
    ShaderProgram::set_mat4(program, "model_matrix",
                            glm::scale(glm::translate(
                                    glm::identity<glm::mat4>(),
                                    position), glm::vec3(1.f / 10.f)));
    ShaderProgram::set_mat4(program, "view_matrix", camera->get_view_matrix());
    ShaderProgram::set_mat4(program, "projection_matrix", camera->get_proj_matrix());

    ShaderProgram::set_vec3(program, "pos_camera", camera->get_camera_position());

    Mesh::render_mesh(mesh_manager->get(MESH_SPHERE));

    ShaderProgram::unuse_shader_program();
}
