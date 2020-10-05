#include "renderer.hpp"
#include "opengl.hpp"

Renderer::Renderer(
        Camera *p_camera, ShaderManager *p_shader_manager, TextureManager *p_texture_manager,
        MeshManager *p_mesh_manager
) :
        camera(p_camera), shader_manager(p_shader_manager), texture_manager(p_texture_manager),
        mesh_manager(p_mesh_manager)
{
    Lines::create_coordinate_axes(&coordinate_mesh);
}

void Renderer::render()
{
    glClear((uint32_t) GL_COLOR_BUFFER_BIT | (uint32_t) GL_DEPTH_BUFFER_BIT);

    if (draw_coordinate) {
        // todo could be a render target if polymorphism between {lines} and {mesh}
        ShaderProgram *program = shader_manager->get(SHADER_LINES);
        ShaderProgram::use_shader_program(program);
        ShaderProgram::set_mat4(program, "modelMatrix", glm::identity<glm::mat4>());
        ShaderProgram::set_mat4(program, "viewMatrix", camera->get_view_matrix());
        ShaderProgram::set_mat4(program, "projectionMatrix", camera->get_proj_matrix());
        Lines::render_lines(&coordinate_mesh);
        ShaderProgram::unuse_shader_program();
    }

    // draw skybox
    ShaderProgram *prog = shader_manager->get(SHADER_DEFAULT); // NB: no shading!
    ShaderProgram::use_shader_program(prog);
    ShaderProgram::set_mat4(prog, "modelMatrix", glm::identity<glm::mat4>());
    ShaderProgram::set_mat4(prog, "viewMatrix", camera->get_view_matrix());
    ShaderProgram::set_mat4(prog, "projectionMatrix", camera->get_proj_matrix());
    Texture::bind_tex(texture_manager->get(TEXTURE_BRICK_DIFF));
    Mesh::render_mesh(mesh_manager->get(MESH_SPHERE));
    Texture::unbind_tex();
    ShaderProgram::unuse_shader_program();
}

void Renderer::toggle_draw_coordinate()
{
    draw_coordinate = !draw_coordinate;
}

Renderer::~Renderer()
{
    Lines::delete_lines(&coordinate_mesh);
}
