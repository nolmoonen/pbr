#ifndef SYSTEM_SHADER_HPP
#define SYSTEM_SHADER_HPP

#include <vector>

#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>

struct ShaderProgram {
    GLuint m_shader_program;

    /**
     * Returns {EXIT_SUCCESS} on success, {EXIT_FAILURE} otherwise.
     * If {EXIT_SUCCESS} is returned, a call to {delete_shader} is required before the executable terminates. */
    static int32_t create_shader_program(
            ShaderProgram *t_shader_program,
            const char *t_vert_shader_text, size_t t_vert_shader_size,
            const char *t_frag_shader_text, size_t t_frag_shader_size
    );

    static void delete_shader_program(ShaderProgram *t_shader_program);

    static void use_shader_program(ShaderProgram *t_shader_program);

    static void unuse_shader_program();

    static void set_vec3(ShaderProgram *shader_program, const char *name, glm::vec3 val);

    static void set_mat4(ShaderProgram *shader_program, const char *name, glm::mat4 val);

    static void set_float(ShaderProgram *shader_program, const char *name, float val);

    static void set_int(ShaderProgram *shader_program, const char *name, int val);

    static void set_vec3_array(ShaderProgram *shader_program, const char *name, std::vector<glm::vec3> vals);
};

struct Shader {
    GLuint shader;

    /**
     * Returns {EXIT_SUCCESS} on success, {EXIT_FAILURE} otherwise.
     * If {EXIT_SUCCESS} is returned, a call to {delete_shader} is required before the executable terminates.
     * If {t_is_vertex} is {true}, create vertex shader. Otherwise, create fragment shader.
     * {t_shader_size} may be 0, consequence is reduced error checking. */
    static int32_t create_shader(Shader *t_shader, const char *t_shader_text, GLint t_shader_size, bool t_is_vertex);

    static void delete_shader(Shader *t_shader);
};

#endif //SYSTEM_SHADER_HPP
