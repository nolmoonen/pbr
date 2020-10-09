#include "renderer.hpp"

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

    scene->render(camera, shader_manager, texture_manager, mesh_manager, debug_mode);

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
