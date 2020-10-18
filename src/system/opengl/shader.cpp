#include "shader.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "../../util/nm_log.hpp"

int32_t ShaderProgram::create_shader_program(
        ShaderProgram *shader_program,
        const char *vert_shader_text, size_t vert_shader_size,
        const char *frag_shader_text, size_t frag_shader_size
)
{
    // shaders
    Shader vert_shader{};
    Shader frag_shader{};

    // create vertex shader
    if (Shader::create_shader(&vert_shader, vert_shader_text, vert_shader_size, true) == EXIT_FAILURE) {
        nm_log::log(LOG_ERROR, "vert shader creation failed\n");

        return EXIT_FAILURE;
    }

    // create fragment shader
    if (Shader::create_shader(&frag_shader, frag_shader_text, frag_shader_size, false) == EXIT_FAILURE) {
        nm_log::log(LOG_ERROR, "frag shader creation failed\n");

        // prevent leaking vertex shader
        Shader::delete_shader(&vert_shader);

        return EXIT_FAILURE;
    }

    // create shader program
    shader_program->shader_program = glCreateProgram();

    glAttachShader(shader_program->shader_program, vert_shader.shader);
    glAttachShader(shader_program->shader_program, frag_shader.shader);

    glLinkProgram(shader_program->shader_program);

    GLint success = GL_FALSE;
    glGetProgramiv(shader_program->shader_program, GL_LINK_STATUS, (int *) &success);
    if (success == GL_FALSE) {
        GLint info_log_length = 0;
        glGetProgramiv(shader_program->shader_program, GL_INFO_LOG_LENGTH, &info_log_length);
        GLchar *info_log = (GLchar *) malloc(info_log_length * sizeof(GLchar));

        if (info_log == NULL) {
            nm_log::log(LOG_ERROR, "could not allocate memory for log info\n");

            glDeleteProgram(shader_program->shader_program);

            Shader::delete_shader(&frag_shader);
            Shader::delete_shader(&vert_shader);

            return EXIT_FAILURE;
        }

        glGetProgramInfoLog(shader_program->shader_program, info_log_length, &info_log_length, info_log);
        info_log[info_log_length - 1] = '\0'; // null terminate
        nm_log::log(LOG_ERROR, "shader program linking failed: %s\n", info_log);
        free(info_log);

        glDeleteProgram(shader_program->shader_program);

        Shader::delete_shader(&frag_shader);
        Shader::delete_shader(&vert_shader);

        return EXIT_FAILURE;
    }

    // detach shaders and delete shaders
    glDetachShader(shader_program->shader_program, frag_shader.shader);
    glDetachShader(shader_program->shader_program, vert_shader.shader);

    Shader::delete_shader(&frag_shader);
    Shader::delete_shader(&vert_shader);

    return EXIT_SUCCESS;
}

void ShaderProgram::delete_shader_program(ShaderProgram *p_shader_program)
{
    glDeleteProgram(p_shader_program->shader_program);
}


void ShaderProgram::use_shader_program(ShaderProgram *p_shader_program)
{
    glUseProgram(p_shader_program->shader_program);
}

void ShaderProgram::unuse_shader_program()
{
    glUseProgram(0);
}

void ShaderProgram::set_vec3(ShaderProgram *p_shader_program, const char *name, glm::vec3 val)
{
    GLint location = glGetUniformLocation(p_shader_program->shader_program, name);
    glUniform3fv(location, 1, glm::value_ptr(val));
}

void ShaderProgram::set_mat4(ShaderProgram *p_shader_program, const char *name, glm::mat4 val)
{
    GLint location = glGetUniformLocation(p_shader_program->shader_program, name);
    // GL_FALSE is passed since glm matrices are column major
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(val));
}

void ShaderProgram::set_float(ShaderProgram *p_shader_program, const char *name, float val)
{
    GLint location = glGetUniformLocation(p_shader_program->shader_program, name);
    glUniform1f(location, val);
}

void ShaderProgram::set_int(ShaderProgram *p_shader_program, const char *name, int val)
{
    GLint location = glGetUniformLocation(p_shader_program->shader_program, name);
    glUniform1i(location, val);
}

void ShaderProgram::set_vec3_array(ShaderProgram *p_shader_program, const char *name, std::vector<glm::vec3> vals)
{
    GLint location = glGetUniformLocation(p_shader_program->shader_program, name);
    glUniform3fv(location, vals.size(), glm::value_ptr(vals.front()));
}

int32_t Shader::create_shader(Shader *p_shader, const char *shader_text, GLint shader_size, bool is_vertex)
{
    if (is_vertex) {
        p_shader->shader = glCreateShader(GL_VERTEX_SHADER);
    } else {
        p_shader->shader = glCreateShader(GL_FRAGMENT_SHADER);
    }

    glShaderSource(p_shader->shader, 1, &shader_text, shader_size ? &shader_size : NULL);
    glCompileShader(p_shader->shader);
    GLint success = GL_FALSE;
    glGetShaderiv(p_shader->shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        GLint info_log_length = 0; // number of characters in info log
        glGetShaderiv(p_shader->shader, GL_INFO_LOG_LENGTH, &info_log_length);
        GLchar *info_log = (GLchar *) malloc(info_log_length * sizeof(GLchar));

        if (info_log == NULL) {
            nm_log::log(LOG_ERROR, "could not allocate memory for info log\n");

            glDeleteShader(p_shader->shader);

            return EXIT_FAILURE;
        }

        glGetShaderInfoLog(p_shader->shader, info_log_length, NULL, info_log);
        info_log[info_log_length - 1] = '\0'; // null terminate
        nm_log::log(LOG_ERROR, "shader compilation failed: %s\n", info_log);
        free(info_log);

        glDeleteShader(p_shader->shader);

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void Shader::delete_shader(Shader *p_shader)
{
    glDeleteShader(p_shader->shader);
}