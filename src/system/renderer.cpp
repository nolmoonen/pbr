#include "renderer.hpp"
#include "material.hpp"

Renderer::Renderer(
        Camera *p_camera, ShaderManager *p_shader_manager, TextureManager *p_texture_manager,
        MeshManager *p_mesh_manager
) :
        camera(p_camera), shader_manager(p_shader_manager), texture_manager(p_texture_manager),
        mesh_manager(p_mesh_manager)
{
    Lines::create_coordinate_axes(&coordinate_mesh);
}

void Renderer::render(Scene *scene)
{
    glClear((uint32_t) GL_COLOR_BUFFER_BIT | (uint32_t) GL_DEPTH_BUFFER_BIT);

    if (debug_mode) render_coordinates(glm::identity<glm::mat4>());

    scene->render(debug_mode);

    window::get_instance().swap_buffers();
}

void Renderer::toggle_draw_coordinate()
{
    debug_mode = !debug_mode;
}

Renderer::~Renderer()
{
    Lines::delete_lines(&coordinate_mesh);
}

void Renderer::render_coordinates(glm::mat4 model_matrix)
{
    ShaderProgram *program = shader_manager->get(SHADER_LINES);
    ShaderProgram::use_shader_program(program);
    ShaderProgram::set_mat4(program, "model_matrix", model_matrix);
    ShaderProgram::set_mat4(program, "view_matrix", camera->get_view_matrix());
    ShaderProgram::set_mat4(program, "projection_matrix", camera->get_proj_matrix());
    Lines::render_lines(&coordinate_mesh);
    ShaderProgram::unuse_shader_program();
}

void Renderer::render_pbr(
        uint32_t mesh_id, uint32_t material_id, std::vector<glm::vec3> positions,
        std::vector<glm::vec3> colors, glm::mat4 model_matrix)
{
    ShaderProgram *program = shader_manager->get(SHADER_PBR);
    ShaderProgram::use_shader_program(program);
    ShaderProgram::set_mat4(program, "model_matrix", model_matrix);
    ShaderProgram::set_mat4(program, "view_matrix", camera->get_view_matrix());
    ShaderProgram::set_mat4(program, "projection_matrix", camera->get_proj_matrix());

    ShaderProgram::set_vec3_array(program, "pos_light", positions);
    ShaderProgram::set_vec3_array(program, "color_light", colors);

    ShaderProgram::set_vec3(program, "pos_camera", camera->get_camera_position());

    Material material{};
    Material::get_material_by_id(material_id, &material);

    ShaderProgram::set_float(program, "metallic", material.metallic);

    ShaderProgram::set_int(program, "texture_diff",
                           (signed) texture_manager->get(material.diffuse)->m_texture_unit - GL_TEXTURE0);
    ShaderProgram::set_int(program, "texture_norm",
                           (signed) texture_manager->get(material.normal)->m_texture_unit - GL_TEXTURE0);
    ShaderProgram::set_int(program, "texture_ao",
                           (signed) texture_manager->get(material.ambient_occlusion)->m_texture_unit - GL_TEXTURE0);
    ShaderProgram::set_int(program, "texture_rough",
                           (signed) texture_manager->get(material.roughness)->m_texture_unit - GL_TEXTURE0);
    ShaderProgram::set_int(program, "texture_disp",
                           (signed) texture_manager->get(material.displacement)->m_texture_unit - GL_TEXTURE0);

    Texture::bind_tex(texture_manager->get(material.diffuse));
    Texture::bind_tex(texture_manager->get(material.normal));
    Texture::bind_tex(texture_manager->get(material.ambient_occlusion));
    Texture::bind_tex(texture_manager->get(material.roughness));
    Texture::bind_tex(texture_manager->get(material.displacement));
    Mesh::render_mesh(mesh_manager->get(mesh_id));
    Texture::unbind_tex();
    ShaderProgram::unuse_shader_program();
}

void Renderer::render_default(uint32_t mesh_id, glm::mat4 model_matrix)
{
    ShaderProgram *program = shader_manager->get(SHADER_DEFAULT);
    ShaderProgram::use_shader_program(program);
    ShaderProgram::set_mat4(program, "model_matrix", model_matrix);
    ShaderProgram::set_mat4(program, "view_matrix", camera->get_view_matrix());
    ShaderProgram::set_mat4(program, "projection_matrix", camera->get_proj_matrix());

    ShaderProgram::set_vec3(program, "pos_camera", camera->get_camera_position());

    Mesh::render_mesh(mesh_manager->get(mesh_id));

    ShaderProgram::unuse_shader_program();
}

void Renderer::render_lines(Lines *lines, glm::mat4 model_matrix)
{
    ShaderProgram *program = shader_manager->get(SHADER_LINES);
    ShaderProgram::use_shader_program(program);
    ShaderProgram::set_mat4(program, "model_matrix", model_matrix);
    ShaderProgram::set_mat4(program, "view_matrix", camera->get_view_matrix());
    ShaderProgram::set_mat4(program, "projection_matrix", camera->get_proj_matrix());
    Lines::render_lines(lines);
    ShaderProgram::unuse_shader_program();
}
