#ifndef SYSTEM_OPENGL_HPP
#define SYSTEM_OPENGL_HPP

#include <cstdlib>

#include <glad/glad.h>

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../util/nm_log.hpp"

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

// todo could get superclass RenderObject or something?
struct Mesh {
    // VAO id
    GLuint m_vertex_array;

    // VBO ids
    GLuint m_buffer_vertex_geom;
    GLuint m_buffer_vertex_tex;
    GLuint m_buffer_vertex_normal;
    GLuint m_buffer_vertex_tangent;
    GLuint m_buffer_vertex_bitangent;
    GLuint m_buffer_index;

    // number of objects to draw
    uint32_t m_index_count;

    /**
     * Returns {@code EXIT_SUCCESS} on success, {@code EXIT_FAILURE} otherwise.
     * If {@code EXIT_SUCCESS} is returned, a call to {@code delete_mesh} is required
     * before the executable terminates.
     *
     * Shaders assume that normals are unitized!
     */
    static void create_mesh(
            Mesh *t_mesh,
            glm::vec3 *t_geom_vertices, glm::vec2 *t_text_vertices, glm::vec3 *t_norm_vertices,
            glm::vec3 *t_tangent_vertices, glm::vec3 *t_bitangent_vertices,
            uint32_t t_vertex_count,
            GLushort *t_indices, uint32_t t_index_count
    );

    static void delete_mesh(Mesh *t_mesh);

    static void render_mesh(Mesh *t_mesh);

    /**
     * Helper function for {create_mesh}.
     * Creates a cubemapped cube of size 2x2x2 centered at (0,0,0). */
    static void create_cube(Mesh *mesh);

    /**
     * Helper function for {create_mesh}. Creates a cubemapped cube of size 2x2x2 centered at (0,0,0).
     * Differs from {create_cube} in the sense that it is intended to be viewed from the inside. */
    static void create_skybox(Mesh *mesh);

    /**
     * Creates a textured unit sphere centered at (0,0,0).
     * http://www.songho.ca/opengl/gl_sphere.html */
    static void create_sphere(Mesh *mesh);
};

// todo could get superclass RenderObject or something?
struct Lines {
    // VAO id
    GLuint m_vertex_array;

    // VBO ids
    GLuint m_buffer_vertex_geom;
    GLuint m_buffer_vertex_color;
    GLuint m_buffer_index;

    // number of objects to draw
    uint32_t m_index_count;

    /** Analogue to create_mesh but for {Lines} instead of {Mesh}. */
    static void create_lines(
            Lines *lines,
            glm::vec3 *geom_vertices, glm::vec3 *color_vertices, uint32_t vertex_count,
            GLushort *indices, uint32_t index_count
    );

    static void delete_lines(Lines *lines);

    static void render_lines(Lines *lines);

    /**
     * Helper function for {create_lines}. Might be a bit overkill to use an element buffer for this.
     * Creates a coordinate system of size 10x10x10 centered at (0,0,0). */
    static void create_coordinate_axes(Lines *lines);

    static void create_line(Lines *line, glm::vec3 dir, glm::vec3 color);

    /** Creates sphere normals, tangents and bitangents. */
    static void create_sphere(Lines *lines);
};

struct Texture {
    /** Id of the texture. */
    GLuint m_tex_id;
    /** Texture unit. */
    GLenum m_texture_unit;

    /** cannot be used with embedded resources */
    static int create_tex_from_file(Texture *tex, const char *tex_file, GLenum texture_unit);

    static int create_tex_from_mem(
            Texture *tex, const char *tex_data, size_t tex_len,
            uint32_t channel_count, uint32_t bit_depth, uint32_t texture_unit);

    static void bind_tex(Texture *tex);

    /** Unbinds the current texture. */
    static void unbind_tex();

    static void delete_tex(Texture *tex);
};

#endif //SYSTEM_OPENGL_HPP