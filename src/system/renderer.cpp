#include "renderer.hpp"

#include "material.hpp"
#include "opengl/texture.hpp"

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

    render_skybox();

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

    Material *material;
    Material::get_material_by_id(material_id, &material);

    material->set(program, texture_manager);

    ShaderProgram::set_int(program, "irradiance_map",
                           (signed) texture_manager->get(cubemap_irradiance)->texture_unit - GL_TEXTURE0);
    ShaderProgram::set_int(program, "pre_filter_map",
                           (signed) texture_manager->get(cubemap_pre_filter)->texture_unit - GL_TEXTURE0);
    ShaderProgram::set_int(program, "brdf_lut",
                           (signed) texture_manager->get(BRDF_LUT)->texture_unit - GL_TEXTURE0);

    material->bind(texture_manager);

    Texture::bind_tex(texture_manager->get(cubemap_irradiance));
    Texture::bind_tex(texture_manager->get(cubemap_pre_filter));
    Texture::bind_tex(texture_manager->get(BRDF_LUT));
    primitive_manager->get(mesh_id)->render_primitive();
    Texture::unbind_tex(texture_manager->get(BRDF_LUT));
    Texture::unbind_tex(texture_manager->get(cubemap_pre_filter));
    Texture::unbind_tex(texture_manager->get(cubemap_irradiance));

    material->unbind(texture_manager);

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

void Renderer::render_widget(glm::vec3 position)
{
    const float CONE_SCALE = 2.f / get_widget_cone_base_radius(position);
    const float CYLINDER_WIDTH_SCALE = 2.f / get_cylinder_radius(position);
    const float CYLINDER_LENGTH_SCALE = get_cylinder_length(position);

    // todo factor out {model_matrix} now we can use {position} and resolve this transformation mess
    glm::mat4 model_matrix = glm::translate(glm::identity<glm::mat4>(), position);

    // x-axis
    render_default(PRIMITIVE_CONE, glm::vec3(1.f, 0.f, 0.f), glm::rotate(
            glm::scale(glm::translate(model_matrix, glm::vec3(CYLINDER_LENGTH_SCALE, 0.f, 0.f)),
                       glm::vec3(1.f / CONE_SCALE)), -(float) M_PI_2, glm::vec3(0.f, 0.f, 1.f)));
    render_default(PRIMITIVE_CYLINDER, glm::vec3(1.f, 0.f, 0.f), glm::translate(
            glm::scale(glm::rotate(model_matrix, -(float) M_PI_2, glm::vec3(0.f, 0.f, 1.f)),
                       glm::vec3(1.f / CYLINDER_WIDTH_SCALE, CYLINDER_LENGTH_SCALE / 2.f, 1.f / CYLINDER_WIDTH_SCALE)),
            glm::vec3(0.f, 1.f, 0.f)));

    // y-axis
    render_default(PRIMITIVE_CONE, glm::vec3(0.f, 1.f, 0.f),
                   glm::scale(glm::translate(model_matrix, glm::vec3(0.f, CYLINDER_LENGTH_SCALE, 0.f)),
                              glm::vec3(1.f / CONE_SCALE)));
    render_default(PRIMITIVE_CYLINDER, glm::vec3(0.f, 1.f, 0.f), glm::translate(glm::scale(model_matrix, glm::vec3(
            1.f / CYLINDER_WIDTH_SCALE, CYLINDER_LENGTH_SCALE / 2.f, 1.f / CYLINDER_WIDTH_SCALE)),
                                                                                glm::vec3(0.f, 1.f, 0.f)));

    // z-axis
    render_default(PRIMITIVE_CONE, glm::vec3(0.f, 0.f, 1.f), glm::rotate(
            glm::scale(glm::translate(model_matrix, glm::vec3(0.f, 0.f, CYLINDER_LENGTH_SCALE)),
                       glm::vec3(1.f / CONE_SCALE)), +(float) M_PI_2, glm::vec3(1.f, 0.f, 0.f)));
    render_default(PRIMITIVE_CYLINDER, glm::vec3(0.f, 0.f, 1.f), glm::translate(
            glm::scale(glm::rotate(model_matrix, (float) M_PI_2, glm::vec3(1.f, 0.f, 0.f)),
                       glm::vec3(1.f / CYLINDER_WIDTH_SCALE, CYLINDER_LENGTH_SCALE / 2.f, 1.f / CYLINDER_WIDTH_SCALE)),
            glm::vec3(0.f, 1.f, 0.f)));
}

float Renderer::get_scale(glm::vec3 position)
{
    glm::vec3 f_p = camera->get_camera_position();
    glm::vec3 f_n = glm::normalize(camera->target - f_p);
    glm::vec3 p = position;
    return glm::dot(p - f_p, f_n);
}

float Renderer::get_widget_cone_base_radius(glm::vec3 position)
{
    return WIDGET_CONE_BASE_RADIUS * get_scale(position);
}

float Renderer::get_widget_cone_height(glm::vec3 position)
{
    return WIDGET_CONE_HEIGHT * get_scale(position);
}

float Renderer::get_cylinder_length(glm::vec3 position)
{
    return CYLINDER_LENGTH * get_scale(position);
}

float Renderer::get_cylinder_radius(glm::vec3 position)
{
    return CYLINDER_RADIUS * get_scale(position);
}

void Renderer::render_skybox()
{
    ShaderProgram *program = shader_manager->get(SHADER_SKYBOX);
    ShaderProgram::use_shader_program(program);
    ShaderProgram::set_mat4(program, "model_matrix", glm::scale(glm::identity<glm::mat4>(), glm::vec3(.5f)));
    ShaderProgram::set_mat4(program, "view_matrix", camera->get_view_matrix());
    ShaderProgram::set_mat4(program, "projection_matrix", camera->get_proj_matrix());

    ShaderProgram::set_int(
            program, "environment_map",
            (signed) texture_manager->get(cubemap)->texture_unit - GL_TEXTURE0);
    Texture::bind_tex(texture_manager->get(cubemap));
    primitive_manager->get(PRIMITIVE_SKYBOX)->render_primitive();
    Texture::unbind_tex(texture_manager->get(cubemap));
    ShaderProgram::unuse_shader_program();
}

void Renderer::switch_skybox(
        TextureType p_cubemap, TextureType p_cubemap_irradiance, TextureType p_cubemap_pre_filter)
{
    cubemap = p_cubemap;
    cubemap_irradiance = p_cubemap_irradiance;
    cubemap_pre_filter = p_cubemap_pre_filter;
}
