#include "opengl.hpp"

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

int32_t ShaderProgram::create_shader_program(
        ShaderProgram *t_shader_program,
        const char *t_vert_shader_text, size_t t_vert_shader_size,
        const char *t_frag_shader_text, size_t t_frag_shader_size
)
{
    // shaders
    Shader vert_shader{};
    Shader frag_shader{};

    // create vertex shader
    if (Shader::create_shader(&vert_shader, t_vert_shader_text, t_vert_shader_size, true) == EXIT_FAILURE) {
        nm_log::log(LOG_ERROR, "vert shader creation failed\n");

        return EXIT_FAILURE;
    }

    // create fragment shader
    if (Shader::create_shader(&frag_shader, t_frag_shader_text, t_frag_shader_size, false) == EXIT_FAILURE) {
        nm_log::log(LOG_ERROR, "frag shader creation failed\n");

        // prevent leaking vertex shader
        Shader::delete_shader(&vert_shader);

        return EXIT_FAILURE;
    }

    // create shader program
    t_shader_program->m_shader_program = glCreateProgram();

    glAttachShader(t_shader_program->m_shader_program, vert_shader.shader);
    glAttachShader(t_shader_program->m_shader_program, frag_shader.shader);

    glLinkProgram(t_shader_program->m_shader_program);

    GLint success = GL_FALSE;
    glGetProgramiv(t_shader_program->m_shader_program, GL_LINK_STATUS, (int *) &success);
    if (success == GL_FALSE) {
        GLint info_log_length = 0;
        glGetProgramiv(t_shader_program->m_shader_program, GL_INFO_LOG_LENGTH, &info_log_length);
        GLchar *info_log = (GLchar *) malloc(info_log_length * sizeof(GLchar));

        if (info_log == NULL) {
            nm_log::log(LOG_ERROR, "could not allocate memory for log info\n");

            glDeleteProgram(t_shader_program->m_shader_program);

            Shader::delete_shader(&frag_shader);
            Shader::delete_shader(&vert_shader);

            return EXIT_FAILURE;
        }

        glGetProgramInfoLog(t_shader_program->m_shader_program, info_log_length, &info_log_length, info_log);
        info_log[info_log_length - 1] = '\0'; // null terminate
        nm_log::log(LOG_ERROR, "shader program linking failed: %s\n", info_log);
        free(info_log);

        glDeleteProgram(t_shader_program->m_shader_program);

        Shader::delete_shader(&frag_shader);
        Shader::delete_shader(&vert_shader);

        return EXIT_FAILURE;
    }

    // detach shaders and delete shaders
    glDetachShader(t_shader_program->m_shader_program, frag_shader.shader);
    glDetachShader(t_shader_program->m_shader_program, vert_shader.shader);

    Shader::delete_shader(&frag_shader);
    Shader::delete_shader(&vert_shader);

    return EXIT_SUCCESS;
}

void ShaderProgram::delete_shader_program(ShaderProgram *t_shader_program)
{
    glDeleteProgram(t_shader_program->m_shader_program);
}


void ShaderProgram::use_shader_program(ShaderProgram *t_shader_program)
{
    glUseProgram(t_shader_program->m_shader_program);
}

void ShaderProgram::unuse_shader_program()
{
    glUseProgram(0);
}

void ShaderProgram::set_vec3(ShaderProgram *shader_program, const char *name, glm::vec3 val)
{
    GLint location = glGetUniformLocation(shader_program->m_shader_program, name);
    glUniform3fv(location, 1, glm::value_ptr(val));
}

void ShaderProgram::set_mat4(ShaderProgram *shader_program, const char *name, glm::mat4 val)
{
    GLint location = glGetUniformLocation(shader_program->m_shader_program, name);
    // GL_FALSE is passed since glm matrices are column major
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(val));
}

int32_t Shader::create_shader(Shader *t_shader, const char *t_shader_text, GLint t_shader_size, bool t_is_vertex)
{
    if (t_is_vertex) {
        t_shader->shader = glCreateShader(GL_VERTEX_SHADER);
    } else {
        t_shader->shader = glCreateShader(GL_FRAGMENT_SHADER);
    }

    glShaderSource(t_shader->shader, 1, &t_shader_text, t_shader_size ? &t_shader_size : NULL);
    glCompileShader(t_shader->shader);
    GLint success = GL_FALSE;
    glGetShaderiv(t_shader->shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        GLint info_log_length = 0; // number of characters in info log
        glGetShaderiv(t_shader->shader, GL_INFO_LOG_LENGTH, &info_log_length);
        GLchar *info_log = (GLchar *) malloc(info_log_length * sizeof(GLchar));

        if (info_log == NULL) {
            nm_log::log(LOG_ERROR, "could not allocate memory for info log\n");

            glDeleteShader(t_shader->shader);

            return EXIT_FAILURE;
        }

        glGetShaderInfoLog(t_shader->shader, info_log_length, NULL, info_log);
        info_log[info_log_length - 1] = '\0'; // null terminate
        nm_log::log(LOG_ERROR, "shader compilation failed: %s\n", info_log);
        free(info_log);

        glDeleteShader(t_shader->shader);

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void Shader::delete_shader(Shader *t_shader)
{
    glDeleteShader(t_shader->shader);
}

void Mesh::create_mesh(
        Mesh *t_mesh,
        glm::vec3 *t_geom_vertices, glm::vec2 *t_text_vertices, glm::vec3 *t_norm_vertices, uint32_t t_vertex_count,
        GLushort *t_indices, uint32_t t_index_count
)
{
    // create VAO
    glGenVertexArrays(1, &t_mesh->m_vertex_array);
    glBindVertexArray(t_mesh->m_vertex_array);

    // create geometric vertex VBO
    glGenBuffers(1, &t_mesh->m_buffer_vertex_geom);
    glBindBuffer(GL_ARRAY_BUFFER, t_mesh->m_buffer_vertex_geom);
    glBufferData(GL_ARRAY_BUFFER, t_vertex_count * sizeof(glm::vec3), t_geom_vertices, GL_STATIC_DRAW);
    // index = 0, size = 3
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);

    // create texture vertex VBO
    glGenBuffers(1, &t_mesh->m_buffer_vertex_tex);
    glBindBuffer(GL_ARRAY_BUFFER, t_mesh->m_buffer_vertex_tex);
    glBufferData(GL_ARRAY_BUFFER, t_vertex_count * sizeof(glm::vec2), t_text_vertices, GL_STATIC_DRAW);
    // index = 1, size = 2
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);

    // create vertex normal VBO
    glGenBuffers(1, &t_mesh->m_buffer_vertex_normal);
    glBindBuffer(GL_ARRAY_BUFFER, t_mesh->m_buffer_vertex_normal);
    glBufferData(GL_ARRAY_BUFFER, t_vertex_count * sizeof(glm::vec3), t_norm_vertices, GL_STATIC_DRAW);
    // index = 3, size = 3
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);

    // create index VBO
    glGenBuffers(1, &t_mesh->m_buffer_index);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, t_mesh->m_buffer_index);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, t_index_count * sizeof(GLushort), t_indices, GL_STATIC_DRAW);

    glBindVertexArray(0);

    t_mesh->m_index_count = t_index_count;
}

void Mesh::delete_mesh(Mesh *t_mesh)
{
    glDeleteBuffers(1, &t_mesh->m_buffer_index);
    t_mesh->m_buffer_index = 0;
    glDeleteBuffers(1, &t_mesh->m_buffer_vertex_normal);
    t_mesh->m_buffer_vertex_normal = 0;
    glDeleteBuffers(1, &t_mesh->m_buffer_vertex_tex);
    t_mesh->m_buffer_vertex_tex = 0;
    glDeleteBuffers(1, &t_mesh->m_buffer_vertex_geom);
    t_mesh->m_buffer_vertex_geom = 0;

    glDeleteVertexArrays(1, &t_mesh->m_vertex_array);
}

void Mesh::render_mesh(Mesh *t_mesh)
{
    glBindVertexArray(t_mesh->m_vertex_array);

    glDrawElements(GL_TRIANGLES, t_mesh->m_index_count, GL_UNSIGNED_SHORT, (void *) 0);

    glBindVertexArray(0);
}

void Mesh::create_cube(Mesh *mesh)
{
    const float SIZE = 1.f;

    uint32_t vertex_count = 6 * 4; // every side four, due to normals and tex coordinates
    glm::vec3 geom_vertices[vertex_count];
    glm::vec2 tex_vertices[vertex_count];
    glm::vec3 norm_vertices[vertex_count];
    uint32_t index_count = 6 * 6; // each side two triangles
    GLushort indices[index_count];

    // todo the z-axis in this picture is wrong
    // topology
    // (i) = index of side
    //  j  = index of vertex on side
    //
    // +         (4) top            +
    // y       1-------0           z
    // ^      /|  (2) /|          ^
    // |     2-------3 | (1)     /
    // o (3) | 3-----|-2        o
    // |     |/ (0)  |/        /
    // v     0-------1        v
    // y        (5) bottom   z
    // - - x < - o - > x +  -

    // geometry coordinates
    //   6-------7
    //  /|      /|
    // 2-------3 |
    // | 4-----|-5
    // |/      |/
    // 0-------1

    // texture coordinates
    //         +-----+
    //         | +y  |
    //   +-----+-----+-----+-----+
    // + | -x  | +z  | +x  | -z  |
    // v +-----+-----+-----+-----+
    // ^       | -y  |
    // |       +-----+
    // o - > u +

    // vertices
    const glm::vec3 V0 = glm::vec3(-SIZE, -SIZE, -SIZE);
    const glm::vec3 V1 = glm::vec3(+SIZE, -SIZE, -SIZE);
    const glm::vec3 V2 = glm::vec3(-SIZE, +SIZE, -SIZE);
    const glm::vec3 V3 = glm::vec3(+SIZE, +SIZE, -SIZE);
    const glm::vec3 V4 = glm::vec3(-SIZE, -SIZE, +SIZE);
    const glm::vec3 V5 = glm::vec3(+SIZE, -SIZE, +SIZE);
    const glm::vec3 V6 = glm::vec3(-SIZE, +SIZE, +SIZE);
    const glm::vec3 V7 = glm::vec3(+SIZE, +SIZE, +SIZE);

    // normals
    const glm::vec3 N0 = glm::vec3(+0.f, +0.f, -1.f);
    const glm::vec3 N1 = glm::vec3(+1.f, +0.f, +0.f);
    const glm::vec3 N2 = glm::vec3(+0.f, +0.f, +1.f);
    const glm::vec3 N3 = glm::vec3(-1.f, +0.f, +0.f);
    const glm::vec3 N4 = glm::vec3(+0.f, +1.f, +0.f);
    const glm::vec3 N5 = glm::vec3(+0.f, -1.f, +0.f);

    const float F13 = 1.f / 3.f;
    const float F23 = 2.f / 3.f;

    // (0) -z
    uint32_t i = 0;
    geom_vertices[i * 4 + 0] = V0;
    geom_vertices[i * 4 + 1] = V1;
    geom_vertices[i * 4 + 2] = V2;
    geom_vertices[i * 4 + 3] = V3;
    tex_vertices[i * 4 + 0] = glm::vec2(1.f, F13);
    tex_vertices[i * 4 + 1] = glm::vec2(.75f, F13);
    tex_vertices[i * 4 + 2] = glm::vec2(1.f, F23);
    tex_vertices[i * 4 + 3] = glm::vec2(.75f, F23);
    norm_vertices[i * 4 + 0] = N0;
    norm_vertices[i * 4 + 1] = N0;
    norm_vertices[i * 4 + 2] = N0;
    norm_vertices[i * 4 + 3] = N0;
    indices[i * 6 + 0] = i * 4 + 0;
    indices[i * 6 + 1] = i * 4 + 3;
    indices[i * 6 + 2] = i * 4 + 1;
    indices[i * 6 + 3] = i * 4 + 0;
    indices[i * 6 + 4] = i * 4 + 2;
    indices[i * 6 + 5] = i * 4 + 3;

    // (1) +x
    i++;
    geom_vertices[i * 4 + 0] = V7;
    geom_vertices[i * 4 + 1] = V1;
    geom_vertices[i * 4 + 2] = V5;
    geom_vertices[i * 4 + 3] = V3;
    tex_vertices[i * 4 + 0] = glm::vec2(.5f, F23);
    tex_vertices[i * 4 + 1] = glm::vec2(.75f, F13);
    tex_vertices[i * 4 + 2] = glm::vec2(.5f, F13);
    tex_vertices[i * 4 + 3] = glm::vec2(.75f, F23);
    norm_vertices[i * 4 + 0] = N1;
    norm_vertices[i * 4 + 1] = N1;
    norm_vertices[i * 4 + 2] = N1;
    norm_vertices[i * 4 + 3] = N1;
    indices[i * 6 + 0] = i * 4 + 1;
    indices[i * 6 + 1] = i * 4 + 0;
    indices[i * 6 + 2] = i * 4 + 2;
    indices[i * 6 + 3] = i * 4 + 1;
    indices[i * 6 + 4] = i * 4 + 3;
    indices[i * 6 + 5] = i * 4 + 0;

    // (2) +z
    i++;
    geom_vertices[i * 4 + 0] = V7;
    geom_vertices[i * 4 + 1] = V6;
    geom_vertices[i * 4 + 2] = V5;
    geom_vertices[i * 4 + 3] = V4;
    tex_vertices[i * 4 + 0] = glm::vec2(.5f, F23);
    tex_vertices[i * 4 + 1] = glm::vec2(.25f, F23);
    tex_vertices[i * 4 + 2] = glm::vec2(.5f, F13);
    tex_vertices[i * 4 + 3] = glm::vec2(.25f, F13);
    norm_vertices[i * 4 + 0] = N2;
    norm_vertices[i * 4 + 1] = N2;
    norm_vertices[i * 4 + 2] = N2;
    norm_vertices[i * 4 + 3] = N2;
    indices[i * 6 + 0] = i * 4 + 2;
    indices[i * 6 + 1] = i * 4 + 1;
    indices[i * 6 + 2] = i * 4 + 3;
    indices[i * 6 + 3] = i * 4 + 2;
    indices[i * 6 + 4] = i * 4 + 0;
    indices[i * 6 + 5] = i * 4 + 1;

    // (3) -x
    i++;
    geom_vertices[i * 4 + 0] = V0;
    geom_vertices[i * 4 + 1] = V6;
    geom_vertices[i * 4 + 2] = V2;
    geom_vertices[i * 4 + 3] = V4;
    tex_vertices[i * 4 + 0] = glm::vec2(0.f, F13);
    tex_vertices[i * 4 + 1] = glm::vec2(.25f, F23);
    tex_vertices[i * 4 + 2] = glm::vec2(0.f, F23);
    tex_vertices[i * 4 + 3] = glm::vec2(.25f, F13);
    norm_vertices[i * 4 + 0] = N3;
    norm_vertices[i * 4 + 1] = N3;
    norm_vertices[i * 4 + 2] = N3;
    norm_vertices[i * 4 + 3] = N3;
    indices[i * 6 + 0] = i * 4 + 3;
    indices[i * 6 + 1] = i * 4 + 2;
    indices[i * 6 + 2] = i * 4 + 0;
    indices[i * 6 + 3] = i * 4 + 3;
    indices[i * 6 + 4] = i * 4 + 1;
    indices[i * 6 + 5] = i * 4 + 2;

    // (4) +y
    i++;
    geom_vertices[i * 4 + 0] = V7;
    geom_vertices[i * 4 + 1] = V6;
    geom_vertices[i * 4 + 2] = V2;
    geom_vertices[i * 4 + 3] = V3;
    tex_vertices[i * 4 + 0] = glm::vec2(.5f, F23);
    tex_vertices[i * 4 + 1] = glm::vec2(.25f, F23);
    tex_vertices[i * 4 + 2] = glm::vec2(.25f, 1.f);
    tex_vertices[i * 4 + 3] = glm::vec2(.5f, 1.f);
    norm_vertices[i * 4 + 0] = N4;
    norm_vertices[i * 4 + 1] = N4;
    norm_vertices[i * 4 + 2] = N4;
    norm_vertices[i * 4 + 3] = N4;
    indices[i * 6 + 0] = i * 4 + 0;
    indices[i * 6 + 1] = i * 4 + 2;
    indices[i * 6 + 2] = i * 4 + 1;
    indices[i * 6 + 3] = i * 4 + 0;
    indices[i * 6 + 4] = i * 4 + 3;
    indices[i * 6 + 5] = i * 4 + 2;

    // (5) -y
    i++;
    geom_vertices[i * 4 + 0] = V0;
    geom_vertices[i * 4 + 1] = V1;
    geom_vertices[i * 4 + 2] = V5;
    geom_vertices[i * 4 + 3] = V4;
    tex_vertices[i * 4 + 0] = glm::vec2(.25f, 0.f);
    tex_vertices[i * 4 + 1] = glm::vec2(.5f, 0.f);
    tex_vertices[i * 4 + 2] = glm::vec2(.5f, F13);
    tex_vertices[i * 4 + 3] = glm::vec2(.25f, F13);
    norm_vertices[i * 4 + 0] = N5;
    norm_vertices[i * 4 + 1] = N5;
    norm_vertices[i * 4 + 2] = N5;
    norm_vertices[i * 4 + 3] = N5;
    indices[i * 6 + 0] = i * 4 + 2;
    indices[i * 6 + 1] = i * 4 + 3;
    indices[i * 6 + 2] = i * 4 + 0;
    indices[i * 6 + 3] = i * 4 + 2;
    indices[i * 6 + 4] = i * 4 + 0;
    indices[i * 6 + 5] = i * 4 + 1;

    create_mesh(
            mesh,
            geom_vertices, tex_vertices, norm_vertices, vertex_count,
            indices, index_count
    );
}

void Mesh::create_skybox(Mesh *mesh)
{
    // NB: basically {create_cube} with different texture coordinates and differently ordered indices
    const float SIZE = 1.f;

    uint32_t vertex_count = 6 * 4;
    glm::vec3 geom_vertices[vertex_count];
    glm::vec2 tex_vertices[vertex_count];
    glm::vec3 norm_vertices[vertex_count];
    uint32_t index_count = 6 * 6;
    GLushort indices[index_count];

    const glm::vec3 V0 = glm::vec3(-SIZE, -SIZE, -SIZE);
    const glm::vec3 V1 = glm::vec3(+SIZE, -SIZE, -SIZE);
    const glm::vec3 V2 = glm::vec3(-SIZE, +SIZE, -SIZE);
    const glm::vec3 V3 = glm::vec3(+SIZE, +SIZE, -SIZE);
    const glm::vec3 V4 = glm::vec3(-SIZE, -SIZE, +SIZE);
    const glm::vec3 V5 = glm::vec3(+SIZE, -SIZE, +SIZE);
    const glm::vec3 V6 = glm::vec3(-SIZE, +SIZE, +SIZE);
    const glm::vec3 V7 = glm::vec3(+SIZE, +SIZE, +SIZE);

    const glm::vec3 N0 = glm::vec3(+0.f, +0.f, -1.f);
    const glm::vec3 N1 = glm::vec3(+1.f, +0.f, +0.f);
    const glm::vec3 N2 = glm::vec3(+0.f, +0.f, +1.f);
    const glm::vec3 N3 = glm::vec3(-1.f, +0.f, +0.f);
    const glm::vec3 N4 = glm::vec3(+0.f, +1.f, +0.f);
    const glm::vec3 N5 = glm::vec3(+0.f, -1.f, +0.f);

    const float F13 = 1.f / 3.f;
    const float F23 = 2.f / 3.f;

    // (0) -z
    uint32_t i = 0;
    geom_vertices[i * 4 + 0] = V0;
    geom_vertices[i * 4 + 1] = V1;
    geom_vertices[i * 4 + 2] = V2;
    geom_vertices[i * 4 + 3] = V3;
    tex_vertices[i * 4 + 0] = glm::vec2(1.f, F13);
    tex_vertices[i * 4 + 1] = glm::vec2(.75f, F13);
    tex_vertices[i * 4 + 2] = glm::vec2(1.f, F23);
    tex_vertices[i * 4 + 3] = glm::vec2(.75f, F23);
    norm_vertices[i * 4 + 0] = N0;
    norm_vertices[i * 4 + 1] = N0;
    norm_vertices[i * 4 + 2] = N0;
    norm_vertices[i * 4 + 3] = N0;
    indices[i * 6 + 0] = i * 4 + 0;
    indices[i * 6 + 1] = i * 4 + 1;
    indices[i * 6 + 2] = i * 4 + 3;
    indices[i * 6 + 3] = i * 4 + 0;
    indices[i * 6 + 4] = i * 4 + 3;
    indices[i * 6 + 5] = i * 4 + 2;

    // (1) +x
    i++;
    geom_vertices[i * 4 + 0] = V7;
    geom_vertices[i * 4 + 1] = V1;
    geom_vertices[i * 4 + 2] = V5;
    geom_vertices[i * 4 + 3] = V3;
    tex_vertices[i * 4 + 0] = glm::vec2(.5f, F23);
    tex_vertices[i * 4 + 1] = glm::vec2(.75f, F13);
    tex_vertices[i * 4 + 2] = glm::vec2(.5f, F13);
    tex_vertices[i * 4 + 3] = glm::vec2(.75f, F23);
    norm_vertices[i * 4 + 0] = N1;
    norm_vertices[i * 4 + 1] = N1;
    norm_vertices[i * 4 + 2] = N1;
    norm_vertices[i * 4 + 3] = N1;
    indices[i * 6 + 0] = i * 4 + 1;
    indices[i * 6 + 1] = i * 4 + 2;
    indices[i * 6 + 2] = i * 4 + 0;
    indices[i * 6 + 3] = i * 4 + 1;
    indices[i * 6 + 4] = i * 4 + 0;
    indices[i * 6 + 5] = i * 4 + 3;

    // (2) +z
    i++;
    geom_vertices[i * 4 + 0] = V7;
    geom_vertices[i * 4 + 1] = V6;
    geom_vertices[i * 4 + 2] = V5;
    geom_vertices[i * 4 + 3] = V4;
    tex_vertices[i * 4 + 0] = glm::vec2(.5f, F23);
    tex_vertices[i * 4 + 1] = glm::vec2(.25f, F23);
    tex_vertices[i * 4 + 2] = glm::vec2(.5f, F13);
    tex_vertices[i * 4 + 3] = glm::vec2(.25f, F13);
    norm_vertices[i * 4 + 0] = N2;
    norm_vertices[i * 4 + 1] = N2;
    norm_vertices[i * 4 + 2] = N2;
    norm_vertices[i * 4 + 3] = N2;
    indices[i * 6 + 0] = i * 4 + 2;
    indices[i * 6 + 1] = i * 4 + 3;
    indices[i * 6 + 2] = i * 4 + 1;
    indices[i * 6 + 3] = i * 4 + 2;
    indices[i * 6 + 4] = i * 4 + 1;
    indices[i * 6 + 5] = i * 4 + 0;

    // (3) -x
    i++;
    geom_vertices[i * 4 + 0] = V0;
    geom_vertices[i * 4 + 1] = V6;
    geom_vertices[i * 4 + 2] = V2;
    geom_vertices[i * 4 + 3] = V4;
    tex_vertices[i * 4 + 0] = glm::vec2(0.f, F13);
    tex_vertices[i * 4 + 1] = glm::vec2(.25f, F23);
    tex_vertices[i * 4 + 2] = glm::vec2(0.f, F23);
    tex_vertices[i * 4 + 3] = glm::vec2(.25f, F13);
    norm_vertices[i * 4 + 0] = N3;
    norm_vertices[i * 4 + 1] = N3;
    norm_vertices[i * 4 + 2] = N3;
    norm_vertices[i * 4 + 3] = N3;
    indices[i * 6 + 0] = i * 4 + 3;
    indices[i * 6 + 1] = i * 4 + 0;
    indices[i * 6 + 2] = i * 4 + 2;
    indices[i * 6 + 3] = i * 4 + 3;
    indices[i * 6 + 4] = i * 4 + 2;
    indices[i * 6 + 5] = i * 4 + 1;

    // (4) +y
    i++;
    geom_vertices[i * 4 + 0] = V7;
    geom_vertices[i * 4 + 1] = V6;
    geom_vertices[i * 4 + 2] = V2;
    geom_vertices[i * 4 + 3] = V3;
    tex_vertices[i * 4 + 0] = glm::vec2(.5f, F23);
    tex_vertices[i * 4 + 1] = glm::vec2(.25f, F23);
    tex_vertices[i * 4 + 2] = glm::vec2(.25f, 1.f);
    tex_vertices[i * 4 + 3] = glm::vec2(.5f, 1.f);
    norm_vertices[i * 4 + 0] = N4;
    norm_vertices[i * 4 + 1] = N4;
    norm_vertices[i * 4 + 2] = N4;
    norm_vertices[i * 4 + 3] = N4;
    indices[i * 6 + 0] = i * 4 + 0;
    indices[i * 6 + 1] = i * 4 + 1;
    indices[i * 6 + 2] = i * 4 + 2;
    indices[i * 6 + 3] = i * 4 + 0;
    indices[i * 6 + 4] = i * 4 + 2;
    indices[i * 6 + 5] = i * 4 + 3;

    // (5) -y
    i++;
    geom_vertices[i * 4 + 0] = V0;
    geom_vertices[i * 4 + 1] = V1;
    geom_vertices[i * 4 + 2] = V5;
    geom_vertices[i * 4 + 3] = V4;
    tex_vertices[i * 4 + 0] = glm::vec2(.25f, 0.f);
    tex_vertices[i * 4 + 1] = glm::vec2(.5f, 0.f);
    tex_vertices[i * 4 + 2] = glm::vec2(.5f, F13);
    tex_vertices[i * 4 + 3] = glm::vec2(.25f, F13);
    norm_vertices[i * 4 + 0] = N5;
    norm_vertices[i * 4 + 1] = N5;
    norm_vertices[i * 4 + 2] = N5;
    norm_vertices[i * 4 + 3] = N5;
    indices[i * 6 + 0] = i * 4 + 2;
    indices[i * 6 + 1] = i * 4 + 0;
    indices[i * 6 + 2] = i * 4 + 3;
    indices[i * 6 + 3] = i * 4 + 2;
    indices[i * 6 + 4] = i * 4 + 1;
    indices[i * 6 + 5] = i * 4 + 0;

    create_mesh(
            mesh,
            geom_vertices, tex_vertices, norm_vertices, vertex_count,
            indices, index_count
    );
}

void InstancedMesh::create_instanced_mesh(
        InstancedMesh *t_inst_mesh,
        glm::vec3 *t_geom_vertices, glm::vec2 *t_text_vertices, glm::vec3 *t_norm_vertices, uint32_t t_vertex_count,
        GLushort *t_indices, uint32_t t_index_count,
        glm::mat4 *t_model_matrices, uint32_t t_model_matrix_count
)
{
    Mesh::create_mesh(
            &t_inst_mesh->m_mesh,
            t_geom_vertices, t_text_vertices, t_norm_vertices, t_vertex_count,
            t_indices, t_index_count
    );

    // create vertex normal VBO
    glGenBuffers(1, &t_inst_mesh->m_buffer_model_matrix);
    glBindBuffer(GL_ARRAY_BUFFER, t_inst_mesh->m_buffer_model_matrix);
    glBufferData(GL_ARRAY_BUFFER, t_model_matrix_count * sizeof(glm::mat4), t_model_matrices, GL_STATIC_DRAW);

    glBindVertexArray(t_inst_mesh->m_mesh.m_vertex_array);

    // index = 3, size = 4 * 4, stride = 4 * 0
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *) (sizeof(glm::vec4) * 0));
    // index = 4, size = 4 * 4, stride = 4 * 1
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *) (sizeof(glm::vec4) * 1));
    // index = 5, size = 4 * 4, stride = 4 * 2
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *) (sizeof(glm::vec4) * 2));
    // index = 6, size = 4 * 4, stride = 4 * 3
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *) (sizeof(glm::vec4) * 3));

    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);

    glBindVertexArray(0);

    t_inst_mesh->m_instance_count = t_model_matrix_count;
}

void InstancedMesh::delete_instanced_mesh(InstancedMesh *t_inst_mesh)
{
    glDeleteBuffers(1, &t_inst_mesh->m_buffer_model_matrix);
    t_inst_mesh->m_buffer_model_matrix = 0;
}

void InstancedMesh::render_instanced_mesh(InstancedMesh *t_inst_mesh)
{
    glBindVertexArray(t_inst_mesh->m_mesh.m_vertex_array);

    glDrawElementsInstanced(
            GL_TRIANGLES, t_inst_mesh->m_mesh.m_index_count, GL_UNSIGNED_SHORT, (void *) 0,
            t_inst_mesh->m_instance_count
    );

    glBindVertexArray(0);
}

void Lines::create_lines(
        Lines *lines,
        glm::vec3 *geom_vertices, glm::vec3 *color_vertices, uint32_t vertex_count,
        GLushort *indices, uint32_t index_count
)
{
    // create VAO
    glGenVertexArrays(1, &lines->m_vertex_array);
    glBindVertexArray(lines->m_vertex_array);

    // create geometric vertex VBO
    glGenBuffers(1, &lines->m_buffer_vertex_geom);
    glBindBuffer(GL_ARRAY_BUFFER, lines->m_buffer_vertex_geom);
    glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(glm::vec3), geom_vertices, GL_STATIC_DRAW);
    // index = 0, size = 3
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);

    // create vertex color VBO
    glGenBuffers(1, &lines->m_buffer_vertex_color);
    glBindBuffer(GL_ARRAY_BUFFER, lines->m_buffer_vertex_color);
    glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(glm::vec3), color_vertices, GL_STATIC_DRAW);
    // index = 1, size = 3
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);

    // create index VBO
    glGenBuffers(1, &lines->m_buffer_index);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lines->m_buffer_index);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(GLushort), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);

    lines->m_index_count = index_count;
}

void Lines::delete_lines(Lines *lines)
{
    glDeleteBuffers(1, &lines->m_buffer_index);
    lines->m_buffer_index = 0;
    glDeleteBuffers(1, &lines->m_buffer_vertex_color);
    lines->m_buffer_vertex_color = 0;
    glDeleteBuffers(1, &lines->m_buffer_vertex_geom);
    lines->m_buffer_vertex_geom = 0;

    glDeleteVertexArrays(1, &lines->m_vertex_array);
}

void Lines::render_lines(Lines *lines)
{
    glBindVertexArray(lines->m_vertex_array);

    glDrawElements(GL_LINES, lines->m_index_count, GL_UNSIGNED_SHORT, (void *) 0);

    glBindVertexArray(0);
}

void Lines::create_coordinate_axes(Lines *lines)
{
    const float SIZE = 10.f;
    uint32_t vertex_count = 6 * 2; // six axes
    glm::vec3 geom_vertices[] = {
            glm::vec3(0.f, 0.f, 0.f), glm::vec3(+SIZE, 0.f, 0.f),
            glm::vec3(0.f, 0.f, 0.f), glm::vec3(-SIZE, 0.f, 0.f),
            glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, +SIZE, 0.f),
            glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, -SIZE, 0.f),
            glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, +SIZE),
            glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, -SIZE),
    };
    glm::vec3 color_vertices[] = {
            glm::vec3(1.f, 0.f, 0.f), glm::vec3(1.f, 1.f, 1.f),
            glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f),
            glm::vec3(0.f, 1.f, 0.f), glm::vec3(1.f, 1.f, 1.f),
            glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 0.f),
            glm::vec3(0.f, 0.f, 1.f), glm::vec3(1.f, 1.f, 1.f),
            glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 0.f, 0.f),
    };
    uint32_t index_count = 6 * 2; // six axes
    GLushort indices[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

    create_lines(
            lines,
            geom_vertices, color_vertices, vertex_count,
            indices, index_count
    );
}

void Lines::create_line(Lines *line, glm::vec3 dir, glm::vec3 color)
{
    uint32_t vertex_count = 2;
    glm::vec3 geom_vertices[] = {glm::vec3(0.f), dir};
    glm::vec3 color_vertices[] = {color, color};
    uint32_t index_count = 2;
    GLushort indices[] = {0, 1};

    create_lines(
            line,
            geom_vertices, color_vertices, vertex_count,
            indices, index_count
    );
}

// todo merge better with {create_tex_from_mem}
int Texture::create_tex_from_file(Texture *tex, const char *tex_file, GLenum texture_unit)
{
    glGenTextures(1, &tex->m_tex_id);
    glBindTexture(GL_TEXTURE_2D, tex->m_tex_id);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // load and generate the texture
    int width, height, channel_count;
    unsigned char *data = stbi_load(tex_file, &width, &height, &channel_count, 0);
    if (data) {
        GLenum format;
        if (channel_count == 3) {
            format = GL_RGB;
        } else if (channel_count == 4) {
            format = GL_RGBA;
        } else {
            nm_log::log(LOG_WARN, "unknown texture format, guessing GL_RGBA\n");
            format = GL_RGBA;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        nm_log::log(LOG_ERROR, "failed to load texture: %s\n", tex_file);
        stbi_image_free(data);

        return EXIT_FAILURE;
    }

    stbi_image_free(data);

    tex->m_texture_unit = texture_unit;

    glBindTexture(GL_TEXTURE_2D, 0);

    return EXIT_SUCCESS;
}

int Texture::create_tex_from_mem(
        Texture *tex, const char *tex_data, size_t tex_len, GLenum texture_unit,
        uint32_t channel_count
)
{
    glGenTextures(1, &tex->m_tex_id);
    glBindTexture(GL_TEXTURE_2D, tex->m_tex_id);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // todo make this an option at some point
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // load and generate the texture
    int width, height, channel_count_;
    // NB: first pixel is top-left corner
    unsigned char *data = stbi_load_from_memory(
            (const unsigned char *) tex_data,
            tex_len, &width, &height, &channel_count_, channel_count
    );

    // flip data vertically
    for (uint32_t y = 0; y < height / 2; y++) {
        uint32_t y_ = height - y - 1;
        for (uint32_t x = 0; x < width; x++) {
            for (uint32_t c = 0; c < channel_count; c++) {
                uint8_t tmp = data[y * width * channel_count + x * channel_count + c];
                data[y * width * channel_count + x * channel_count + c] =
                        data[y_ * width * channel_count + x * channel_count + c];
                data[y_ * width * channel_count + x * channel_count + c] = tmp;
            }
        }
    }

    if (data) {
        GLenum format;
        if (channel_count == 3) {
            format = GL_RGB;
        } else if (channel_count == 4) {
            format = GL_RGBA;
        } else {
            nm_log::log(LOG_WARN, "unknown texture format, guessing GL_RGBA\n");
            format = GL_RGBA;
        }

        // NB: first pixel is lower-left corner
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        nm_log::log(LOG_ERROR, "failed to load texture\n");
        stbi_image_free(data);

        return EXIT_FAILURE;
    }

    stbi_image_free(data);

    tex->m_texture_unit = texture_unit;

    glBindTexture(GL_TEXTURE_2D, 0);

    return EXIT_SUCCESS;
}

void Texture::bind_tex(Texture *tex)
{
    glActiveTexture(tex->m_texture_unit);
    glBindTexture(GL_TEXTURE_2D, tex->m_tex_id);
}

void Texture::unbind_tex()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::delete_tex(Texture *tex)
{
    glDeleteTextures(1, &tex->m_tex_id);
}