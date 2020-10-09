#ifndef SYSTEM_LINES_HPP
#define SYSTEM_LINES_HPP

#include <cmath>
#include <glm/geometric.hpp>

#include <glad/glad.h>
#include <glm/vec3.hpp>

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

#endif //SYSTEM_LINES_HPP
