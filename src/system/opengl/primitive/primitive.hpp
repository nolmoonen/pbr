#ifndef SYSTEM_PRIMITIVE_HPP
#define SYSTEM_PRIMITIVE_HPP

#include <glad/glad.h>
#include <glm/vec3.hpp>

/** Describes a primitive with an element buffer. */
class Primitive {
protected:
    GLuint vertex_array = 0;       // VAO id
    GLuint buffer_vertex_geom = 0; // VBO id for geometric data
    GLuint buffer_index = 0;       // VBO id for element data
    uint32_t index_count = 0;      // number of objects to draw

public:
    virtual ~Primitive() = default;

    void create_primitive(
            glm::vec3 *geom_vertices, uint32_t vertex_count,
            GLushort *indices, uint32_t p_index_count);

    virtual void delete_primitive();

    virtual void render_primitive();

    /**
     * Helper functions.
     */

    /** Creates 1x2x1 cone centered at (0,0,0). */
    static Primitive *create_cone();

    static Primitive *create_cylinder();
};

#endif //SYSTEM_PRIMITIVE_HPP
