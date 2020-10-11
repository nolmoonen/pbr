#ifndef SYSTEM_LINES_PRIMITIVE_HPP
#define SYSTEM_LINES_PRIMITIVE_HPP

#include "primitive.hpp"

class LinesPrimitive : public Primitive {
private:
    GLuint buffer_vertex_color = 0;    // VBO id for color data
public:
    ~LinesPrimitive() override = default;

    void create(
            glm::vec3 *geom_vertices, glm::vec3 *color_vertices, uint32_t vertex_count,
            GLushort *indices, uint32_t p_index_count);

    void delete_primitive() override;

    void render_primitive() override;

    /**
     * Helper functions.
     */

    /**
     * Creates a coordinate system of size 10x10x10 centered at (0,0,0). */
    static Primitive *create_coordinate_axes();


    /** Creates sphere normals, tangents and bitangents. */
    static Primitive *create_sphere();
};

#endif //SYSTEM_LINES_PRIMITIVE_HPP
