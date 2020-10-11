#include "primitive.hpp"

#include <cmath>
#include <glm/glm.hpp>

void Primitive::create_primitive(
        glm::vec3 *geom_vertices, uint32_t vertex_count, GLushort *indices,
        uint32_t p_index_count)
{
    // create VAO
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);

    // create geometric vertex VBO
    glGenBuffers(1, &buffer_vertex_geom);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_vertex_geom);
    glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(glm::vec3), geom_vertices, GL_STATIC_DRAW);
    // index = 0, size = 3
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);

    // create index VBO
    glGenBuffers(1, &buffer_index);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_index);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, p_index_count * sizeof(GLushort), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);

    index_count = p_index_count;
}

void Primitive::delete_primitive()
{
    glDeleteBuffers(1, &buffer_index);
    buffer_index = 0;
    glDeleteBuffers(1, &buffer_vertex_geom);
    buffer_vertex_geom = 0;

    glDeleteVertexArrays(1, &vertex_array);
}

void Primitive::render_primitive()
{
    glBindVertexArray(vertex_array);

    glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_SHORT, (void *) 0);

    glBindVertexArray(0);
}

Primitive *Primitive::create_cone()
{
    const uint32_t SECTOR_COUNT = 64;

    const uint32_t VERTEX_COUNT = SECTOR_COUNT + 1; // number of sectors + top vertex
    glm::vec3 geom_vertices[VERTEX_COUNT];

    const float SECTOR_STEP = (2.f * (float) M_PI) / (float) SECTOR_COUNT;

    uint32_t vertex = 0; // vertex index
    geom_vertices[vertex++] = glm::vec3(0.f, 2.f, 0.f);
    const uint32_t INDEX_COUNT = SECTOR_COUNT * 3; // three for every sector
    GLushort indices[INDEX_COUNT];
    uint32_t index = 0;
    for (uint32_t i = 0; i < SECTOR_COUNT; i++) {
        float sector_angle = (float) i * SECTOR_STEP; // from 0 to 2 * pi
        indices[index++] = 0;
        indices[index++] = vertex;
        indices[index++] = i != SECTOR_COUNT - 1 ? vertex + 1 : 1;
        geom_vertices[vertex++] = glm::vec3(cosf(sector_angle), 0, sinf(sector_angle));
    }

    auto *primitive = new Primitive;
    primitive->create_primitive(geom_vertices, VERTEX_COUNT, indices, INDEX_COUNT);
    return (Primitive *) primitive;
}
