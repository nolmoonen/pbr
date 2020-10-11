#ifndef SYSTEM_FULL_PRIMITIVE_HPP
#define SYSTEM_FULL_PRIMITIVE_HPP

#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include "primitive.hpp"

class FullPrimitive : public Primitive {
private:
    GLuint buffer_vertex_tex = 0;       // VBO id for texture data
    GLuint buffer_vertex_normal = 0;    // VBO id for normal data
    GLuint buffer_vertex_tangent = 0;   // VBO id for tangent data
    GLuint buffer_vertex_bitangent = 0; // VBO id for bitangent data
public:
    ~FullPrimitive() override = default;

    void create(
            glm::vec3 *geom_vertices, glm::vec2 *text_vertices, glm::vec3 *norm_vertices,
            glm::vec3 *tangent_vertices, glm::vec3 *bitangent_vertices,
            uint32_t vertex_count,
            GLushort *indices, uint32_t p_index_count);

    void delete_primitive() override;

    void render_primitive() override;

    /**
     * Helper functions.
     */

    /**
     * Creates a cubemapped cube of size 2x2x2 centered at (0,0,0). */
    static Primitive *create_cube();

    /**
     * Creates a cubemapped cube of size 2x2x2 centered at (0,0,0).
     * Differs from {create_cube} in the sense that it is intended to be viewed from the inside. */
    static Primitive *create_skybox();

    /**
     * Creates a textured unit sphere centered at (0,0,0).
     * http://www.songho.ca/opengl/gl_sphere.html */
    static Primitive *create_sphere();
};

#endif //SYSTEM_FULL_PRIMITIVE_HPP
