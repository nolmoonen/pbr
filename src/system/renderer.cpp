#include "renderer.hpp"
#include "material.hpp"

Renderer::Renderer(
        Camera *p_camera, ShaderManager *p_shader_manager, TextureManager *p_texture_manager,
        PrimitiveManager *p_primitive_manager
) :
        camera(p_camera), shader_manager(p_shader_manager), texture_manager(p_texture_manager),
        primitive_manager(p_primitive_manager)
{}

void Renderer::render(Scene *scene)
{
    glClear((uint32_t) GL_COLOR_BUFFER_BIT | (uint32_t) GL_DEPTH_BUFFER_BIT);

    if (debug_mode) {
        render_lines(PRIMITIVE_COORDINATE_SYSTEM, glm::identity<glm::mat4>());
    }

    scene->render(debug_mode);

    window::get_instance().swap_buffers();
}

void Renderer::toggle_draw_coordinate()
{
    debug_mode = !debug_mode;
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
    primitive_manager->get(mesh_id)->render_primitive();
    Texture::unbind_tex();
    ShaderProgram::unuse_shader_program();
}

void Renderer::render_default(uint32_t mesh_id, glm::vec3 color, glm::mat4 model_matrix)
{
    ShaderProgram *program = shader_manager->get(SHADER_DEFAULT);
    ShaderProgram::use_shader_program(program);
    ShaderProgram::set_mat4(program, "model_matrix", model_matrix);
    ShaderProgram::set_mat4(program, "view_matrix", camera->get_view_matrix());
    ShaderProgram::set_mat4(program, "projection_matrix", camera->get_proj_matrix());

    ShaderProgram::set_vec3(program, "pos_camera", camera->get_camera_position());

    ShaderProgram::set_vec3(program, "color", color);

    primitive_manager->get(mesh_id)->render_primitive();

    ShaderProgram::unuse_shader_program();
}

void Renderer::render_lines(uint32_t primitive_id, glm::mat4 model_matrix)
{
    ShaderProgram *program = shader_manager->get(SHADER_LINES);
    ShaderProgram::use_shader_program(program);
    ShaderProgram::set_mat4(program, "model_matrix", model_matrix);
    ShaderProgram::set_mat4(program, "view_matrix", camera->get_view_matrix());
    ShaderProgram::set_mat4(program, "projection_matrix", camera->get_proj_matrix());
    primitive_manager->get(primitive_id)->render_primitive();
    ShaderProgram::unuse_shader_program();
}

void Renderer::render_widget(glm::mat4 model_matrix)
{
    const float CONE_SCALE = 20.f;           // how large the cone is
    const float CYLINDER_LENGTH = 1.f;       // how long the cylinder should be (cone placement is adjusted accordingly)
    const float CYLINDER_WIDTH_SCALE = 45.f; // the girth of the cylinder

    // x-axis
    render_default(PRIMITIVE_CONE, glm::vec3(1.f, 0.f, 0.f),
                   glm::rotate(glm::scale(glm::translate(model_matrix, glm::vec3(CYLINDER_LENGTH, 0.f, 0.f)),
                                          glm::vec3(1.f / CONE_SCALE)), -(float) M_PI_2, glm::vec3(0.f, 0.f, 1.f)));
    render_default(PRIMITIVE_CYLINDER, glm::vec3(1.f, 0.f, 0.f),
                   glm::translate(glm::scale(glm::rotate(model_matrix, -(float) M_PI_2, glm::vec3(0.f, 0.f, 1.f)),
                                             glm::vec3(1.f / CYLINDER_WIDTH_SCALE, CYLINDER_LENGTH / 2.f,
                                                       1.f / CYLINDER_WIDTH_SCALE)), glm::vec3(0.f, 1.f, 0.f)));

    // y-axis
    render_default(PRIMITIVE_CONE, glm::vec3(0.f, 1.f, 0.f),
                   glm::scale(glm::translate(model_matrix, glm::vec3(0.f, CYLINDER_LENGTH, 0.f)),
                              glm::vec3(1.f / CONE_SCALE)));
    render_default(PRIMITIVE_CYLINDER, glm::vec3(0.f, 1.f, 0.f),
                   glm::translate(glm::scale(model_matrix, glm::vec3(1.f / CYLINDER_WIDTH_SCALE, CYLINDER_LENGTH / 2.f,
                                                                     1.f / CYLINDER_WIDTH_SCALE)),
                                  glm::vec3(0.f, 1.f, 0.f)));

    // z-axis
    render_default(PRIMITIVE_CONE, glm::vec3(0.f, 0.f, 1.f),
                   glm::rotate(glm::scale(glm::translate(model_matrix, glm::vec3(0.f, 0.f, CYLINDER_LENGTH)),
                                          glm::vec3(1.f / CONE_SCALE)), +(float) M_PI_2, glm::vec3(1.f, 0.f, 0.f)));
    render_default(PRIMITIVE_CYLINDER, glm::vec3(0.f, 0.f, 1.f),
                   glm::translate(glm::scale(glm::rotate(model_matrix, (float) M_PI_2, glm::vec3(1.f, 0.f, 0.f)),
                                             glm::vec3(1.f / CYLINDER_WIDTH_SCALE, CYLINDER_LENGTH / 2.f,
                                                       1.f / CYLINDER_WIDTH_SCALE)), glm::vec3(0.f, 1.f, 0.f)));
}