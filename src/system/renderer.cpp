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
    Lines::create_sphere(&normals); // todo debug
}

void Renderer::render()
{
    glClear((uint32_t) GL_COLOR_BUFFER_BIT | (uint32_t) GL_DEPTH_BUFFER_BIT);

    if (draw_coordinate) {
        // todo could be a render target if polymorphism between {lines} and {mesh}
        ShaderProgram *program = shader_manager->get(SHADER_LINES);
        ShaderProgram::use_shader_program(program);
        ShaderProgram::set_mat4(program, "model_matrix", glm::identity<glm::mat4>());
        ShaderProgram::set_mat4(program, "view_matrix", camera->get_view_matrix());
        ShaderProgram::set_mat4(program, "projection_matrix", camera->get_proj_matrix());
        Lines::render_lines(&coordinate_mesh);
        Lines::render_lines(&normals); // todo debug
        ShaderProgram::unuse_shader_program();
    }

    // draw sphere
    ShaderProgram *program = shader_manager->get(SHADER_PBR);
    ShaderProgram::use_shader_program(program);
    ShaderProgram::set_mat4(program, "model_matrix", glm::identity<glm::mat4>());
    ShaderProgram::set_mat4(program, "view_matrix", camera->get_view_matrix());
    ShaderProgram::set_mat4(program, "projection_matrix", camera->get_proj_matrix());

    ShaderProgram::set_vec3(program, "pos_light", glm::vec3(1.f, 1.f, 1.f));
    ShaderProgram::set_vec3(program, "pos_camera", camera->get_camera_position());
    ShaderProgram::set_vec3(program, "color_light", glm::vec3(1.f, 1.f, 1.f));

    ShaderProgram::set_float(program, "metallic", 0.f);
    ShaderProgram::set_float(program, "roughness", 5.f);
    ShaderProgram::set_float(program, "ao", 1.f);

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

void Renderer::toggle_draw_coordinate()
{
    draw_coordinate = !draw_coordinate;
}

Renderer::~Renderer()
{
    Lines::delete_lines(&coordinate_mesh);
}
