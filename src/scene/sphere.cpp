#include "sphere.hpp"
#include "../system/opengl.hpp"
#include "../system/shader_manager.hpp"
#include "../system/texture_manager.hpp"

Sphere::Sphere(Scene *scene, const glm::vec3 &position) : SceneObject(scene, position)
{
    Lines::create_sphere(&normals); // todo debug
}

void Sphere::render(
        Camera *camera, ShaderManager *shader_manager, TextureManager *texture_manager,
        MeshManager *mesh_manager,
        bool debug_mode)
{
    if (debug_mode) {
        ShaderProgram *program = shader_manager->get(SHADER_LINES);
        ShaderProgram::use_shader_program(program);
        ShaderProgram::set_mat4(program, "model_matrix", glm::identity<glm::mat4>());
        ShaderProgram::set_mat4(program, "view_matrix", camera->get_view_matrix());
        ShaderProgram::set_mat4(program, "projection_matrix", camera->get_proj_matrix());
        Lines::render_lines(&normals);
        ShaderProgram::unuse_shader_program();
    }

    ShaderProgram *program = shader_manager->get(SHADER_PBR);
    ShaderProgram::use_shader_program(program);
    ShaderProgram::set_mat4(program, "model_matrix", glm::identity<glm::mat4>());
    ShaderProgram::set_mat4(program, "view_matrix", camera->get_view_matrix());
    ShaderProgram::set_mat4(program, "projection_matrix", camera->get_proj_matrix());

    for (auto &light:scene->lights) {
        // todo add multiple lights
        ShaderProgram::set_vec3(program, "pos_light", light.position);
        ShaderProgram::set_vec3(program, "color_light", light.color);
    }

    ShaderProgram::set_vec3(program, "pos_camera", camera->get_camera_position());

    ShaderProgram::set_float(program, "metallic", 0.f);

    ShaderProgram::set_int(program, "texture_diff",
                           (signed) texture_manager->get(TEXTURE_BRICK_DIFF)->m_texture_unit - GL_TEXTURE0);
    ShaderProgram::set_int(program, "texture_norm",
                           (signed) texture_manager->get(TEXTURE_BRICK_NORM)->m_texture_unit - GL_TEXTURE0);
    ShaderProgram::set_int(program, "texture_ao",
                           (signed) texture_manager->get(TEXTURE_BRICK_AO)->m_texture_unit - GL_TEXTURE0);
    ShaderProgram::set_int(program, "texture_rough",
                           (signed) texture_manager->get(TEXTURE_BRICK_ROUGH)->m_texture_unit - GL_TEXTURE0);

    Texture::bind_tex(texture_manager->get(TEXTURE_BRICK_DIFF));
    Texture::bind_tex(texture_manager->get(TEXTURE_BRICK_NORM));
    Texture::bind_tex(texture_manager->get(TEXTURE_BRICK_AO));
    Texture::bind_tex(texture_manager->get(TEXTURE_BRICK_ROUGH));
    Mesh::render_mesh(mesh_manager->get(MESH_SPHERE));
    Texture::unbind_tex();
    ShaderProgram::unuse_shader_program();
}