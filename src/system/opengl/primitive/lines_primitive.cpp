#include "lines_primitive.hpp"

#include <cmath>

#include <glm/glm.hpp>

void LinesPrimitive::create(
        glm::vec3 *geom_vertices, glm::vec3 *color_vertices, uint32_t vertex_count,
        GLushort *indices, uint32_t p_index_count)
{
    Primitive::create_primitive(geom_vertices, vertex_count, indices, p_index_count);

    glBindVertexArray(vertex_array);

    // create vertex color VBO
    glGenBuffers(1, &buffer_vertex_color);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_vertex_color);
    glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(glm::vec3), color_vertices, GL_STATIC_DRAW);
    // index = 1, size = 3
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);

    glBindVertexArray(0);
}

void LinesPrimitive::delete_primitive()
{
    glDeleteBuffers(1, &buffer_vertex_color);
    buffer_vertex_color = 0;

    Primitive::delete_primitive();
}

void LinesPrimitive::render_primitive()
{
    // fully override behavior to render lines
    glBindVertexArray(vertex_array);

    glDrawElements(GL_LINES, index_count, GL_UNSIGNED_SHORT, (void *) 0);

    glBindVertexArray(0);
}

Primitive *LinesPrimitive::create_coordinate_axes()
{
    const float SIZE = 5.f;
    uint32_t vertex_count = 3 * 2; // three axes
    glm::vec3 geom_vertices[] = {
            glm::vec3(0.f, 0.f, 0.f), glm::vec3(+SIZE, 0.f, 0.f),
            glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, +SIZE, 0.f),
            glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, +SIZE),
    };
    glm::vec3 color_vertices[] = {
            glm::vec3(1.f, 0.f, 0.f), glm::vec3(1.f, 0.f, 0.f),
            glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 1.f, 0.f),
            glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 0.f, 1.f),
    };
    uint32_t index_count = 3 * 2; // three axes
    GLushort indices[] = {0, 1, 2, 3, 4, 5};

    auto *lines_primitive = new LinesPrimitive;
    lines_primitive->create(
            geom_vertices, color_vertices, vertex_count,
            indices, index_count);
    return (Primitive *) lines_primitive;
}

Primitive *LinesPrimitive::create_sphere()
{
    // NB: code is mirrored from {Mesh:create_sphere}
    const uint32_t STACK_COUNT = 64;
    const uint32_t SECTOR_COUNT = 64;
    const float SCALE = 25.f;

    const uint32_t SPHERE_VERTICES = (STACK_COUNT + 1) * (SECTOR_COUNT + 1);
    // 1 for vertex, 1 for normal, 1 for tangent, 1 for bitangent
    const uint32_t VERTEX_COUNT = SPHERE_VERTICES * 4;
    glm::vec3 geom_vertices[VERTEX_COUNT];
    glm::vec3 color_vertices[VERTEX_COUNT];
    const uint32_t INDEX_COUNT = SPHERE_VERTICES * 6; // three lines per vertex
    GLushort indices[INDEX_COUNT];

    const float STACK_STEP = M_PI / (float) STACK_COUNT;
    const float SECTOR_STEP = (2.f * (float) M_PI) / (float) SECTOR_COUNT;

    uint32_t vertex = 0;
    uint32_t index = 0;
    for (uint32_t i = 0; i < STACK_COUNT + 1; i++) {
        float stack_angle = (float) M_PI_2 - (float) i * STACK_STEP;
        float xz = cosf(stack_angle);
        float y = sinf(stack_angle);

        for (uint32_t j = 0; j < SECTOR_COUNT + 1; j++) {
            float sector_angle = (float) j * SECTOR_STEP;

            float cos_theta = cosf(sector_angle);
            float sin_theta = sinf(sector_angle);
            float x = xz * cos_theta;
            float z = xz * sin_theta;
            glm::vec3 geom = glm::vec3(x, y, z); // vertex
            geom_vertices[vertex + 0] = geom;
            color_vertices[vertex + 0] = glm::vec3(1.f);
            geom_vertices[vertex + 1] = geom + geom / SCALE;
            color_vertices[vertex + 1] = glm::vec3(0.f, 0.f, 1.f);
            glm::vec3 tangent = glm::vec3(sin_theta, 0.f, -cos_theta);
            geom_vertices[vertex + 2] = geom + tangent / SCALE;
            color_vertices[vertex + 2] = glm::vec3(1.f, 0.f, 0.f);
            glm::vec3 bitangent = glm::normalize(glm::vec3(-y * cos_theta, xz, -y * sin_theta));
            geom_vertices[vertex + 3] = geom + bitangent / SCALE;
            color_vertices[vertex + 3] = glm::vec3(0.f, 1.f, 0.f);

            indices[index++] = vertex + 0;
            indices[index++] = vertex + 1;
            indices[index++] = vertex + 0;
            indices[index++] = vertex + 2;
            indices[index++] = vertex + 0;
            indices[index++] = vertex + 3;
            vertex += 4;
        }
    }

    auto *lines_primitive = new LinesPrimitive;
    lines_primitive->create(
            geom_vertices, color_vertices, VERTEX_COUNT,
            indices, INDEX_COUNT);
    return (Primitive *) lines_primitive;
}
