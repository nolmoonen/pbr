#include "shader.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "../../util/nm_log.hpp"

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

void ShaderProgram::set_float(ShaderProgram *shader_program, const char *name, float val)
{
    GLint location = glGetUniformLocation(shader_program->m_shader_program, name);
    glUniform1f(location, val);
}

void ShaderProgram::set_int(ShaderProgram *shader_program, const char *name, int val)
{
    GLint location = glGetUniformLocation(shader_program->m_shader_program, name);
    glUniform1i(location, val);
}

void ShaderProgram::set_vec3_array(ShaderProgram *shader_program, const char *name, std::vector<glm::vec3> vals)
{
    GLint location = glGetUniformLocation(shader_program->m_shader_program, name);
    glUniform3fv(location, vals.size(), glm::value_ptr(vals.front()));
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