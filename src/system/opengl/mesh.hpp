#ifndef SYSTEM_MESH_HPP
#define SYSTEM_MESH_HPP

#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

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

#endif //SYSTEM_MESH_HPP
