#include "mesh.hpp"

#include <cmath>

#include <glm/geometric.hpp>

void Mesh::create_mesh(
        Mesh *t_mesh,
        glm::vec3 *t_geom_vertices, glm::vec2 *t_text_vertices, glm::vec3 *t_norm_vertices,
        glm::vec3 *t_tangent_vertices, glm::vec3 *t_bitangent_vertices,
        uint32_t t_vertex_count,
        GLushort *t_indices, uint32_t t_index_count
)
{
    // create VAO
    glGenVertexArrays(1, &t_mesh->m_vertex_array);
    glBindVertexArray(t_mesh->m_vertex_array);

    // create geometric vertex VBO
    glGenBuffers(1, &t_mesh->m_buffer_vertex_geom);
    glBindBuffer(GL_ARRAY_BUFFER, t_mesh->m_buffer_vertex_geom);
    glBufferData(GL_ARRAY_BUFFER, t_vertex_count * sizeof(glm::vec3), t_geom_vertices, GL_STATIC_DRAW);
    // index = 0, size = 3
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);

    // create texture vertex VBO
    glGenBuffers(1, &t_mesh->m_buffer_vertex_tex);
    glBindBuffer(GL_ARRAY_BUFFER, t_mesh->m_buffer_vertex_tex);
    glBufferData(GL_ARRAY_BUFFER, t_vertex_count * sizeof(glm::vec2), t_text_vertices, GL_STATIC_DRAW);
    // index = 1, size = 2
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);

    // create vertex normal VBO
    glGenBuffers(1, &t_mesh->m_buffer_vertex_normal);
    glBindBuffer(GL_ARRAY_BUFFER, t_mesh->m_buffer_vertex_normal);
    glBufferData(GL_ARRAY_BUFFER, t_vertex_count * sizeof(glm::vec3), t_norm_vertices, GL_STATIC_DRAW);
    // index = 3, size = 3
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);

    // create vertex tangent VBO
    glGenBuffers(1, &t_mesh->m_buffer_vertex_tangent);
    glBindBuffer(GL_ARRAY_BUFFER, t_mesh->m_buffer_vertex_tangent);
    glBufferData(GL_ARRAY_BUFFER, t_vertex_count * sizeof(glm::vec3), t_tangent_vertices, GL_STATIC_DRAW);
    // index = 3, size = 3
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);

    // create vertex bitangent VBO
    glGenBuffers(1, &t_mesh->m_buffer_vertex_bitangent);
    glBindBuffer(GL_ARRAY_BUFFER, t_mesh->m_buffer_vertex_bitangent);
    glBufferData(GL_ARRAY_BUFFER, t_vertex_count * sizeof(glm::vec3), t_bitangent_vertices, GL_STATIC_DRAW);
    // index = 3, size = 3
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);

    // create index VBO
    glGenBuffers(1, &t_mesh->m_buffer_index);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, t_mesh->m_buffer_index);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, t_index_count * sizeof(GLushort), t_indices, GL_STATIC_DRAW);

    glBindVertexArray(0);

    t_mesh->m_index_count = t_index_count;
}

void Mesh::delete_mesh(Mesh *t_mesh)
{
    glDeleteBuffers(1, &t_mesh->m_buffer_index);
    t_mesh->m_buffer_index = 0;
    glDeleteBuffers(1, &t_mesh->m_buffer_vertex_bitangent);
    t_mesh->m_buffer_vertex_bitangent = 0;
    glDeleteBuffers(1, &t_mesh->m_buffer_vertex_tangent);
    t_mesh->m_buffer_vertex_tangent = 0;
    glDeleteBuffers(1, &t_mesh->m_buffer_vertex_normal);
    t_mesh->m_buffer_vertex_normal = 0;
    glDeleteBuffers(1, &t_mesh->m_buffer_vertex_tex);
    t_mesh->m_buffer_vertex_tex = 0;
    glDeleteBuffers(1, &t_mesh->m_buffer_vertex_geom);
    t_mesh->m_buffer_vertex_geom = 0;

    glDeleteVertexArrays(1, &t_mesh->m_vertex_array);
}

void Mesh::render_mesh(Mesh *t_mesh)
{
    glBindVertexArray(t_mesh->m_vertex_array);

    glDrawElements(GL_TRIANGLES, t_mesh->m_index_count, GL_UNSIGNED_SHORT, (void *) 0);

    glBindVertexArray(0);
}

void Mesh::create_cube(Mesh *mesh)
{
    const float SIZE = 1.f;

    const uint32_t vertex_count = 6 * 4; // every side four, due to normals and tex coordinates
    glm::vec3 geom_vertices[vertex_count];
    glm::vec2 tex_vertices[vertex_count];
    glm::vec3 norm_vertices[vertex_count];
    const uint32_t index_count = 6 * 6; // each side two triangles
    GLushort indices[index_count];

    // todo the z-axis in this picture is wrong
    // topology
    // (i) = index of side
    //  j  = index of vertex on side
    //
    // +         (4) top            +
    // y       1-------0           z
    // ^      /|  (2) /|          ^
    // |     2-------3 | (1)     /
    // o (3) | 3-----|-2        o
    // |     |/ (0)  |/        /
    // v     0-------1        v
    // y        (5) bottom   z
    // - - x < - o - > x +  -

    // geometry coordinates
    //   6-------7
    //  /|      /|
    // 2-------3 |
    // | 4-----|-5
    // |/      |/
    // 0-------1

    // texture coordinates
    //         +-----+
    //         | +y  |
    //   +-----+-----+-----+-----+
    // + | -x  | +z  | +x  | -z  |
    // v +-----+-----+-----+-----+
    // ^       | -y  |
    // |       +-----+
    // o - > u +

    // vertices
    const glm::vec3 V0 = glm::vec3(-SIZE, -SIZE, -SIZE);
    const glm::vec3 V1 = glm::vec3(+SIZE, -SIZE, -SIZE);
    const glm::vec3 V2 = glm::vec3(-SIZE, +SIZE, -SIZE);
    const glm::vec3 V3 = glm::vec3(+SIZE, +SIZE, -SIZE);
    const glm::vec3 V4 = glm::vec3(-SIZE, -SIZE, +SIZE);
    const glm::vec3 V5 = glm::vec3(+SIZE, -SIZE, +SIZE);
    const glm::vec3 V6 = glm::vec3(-SIZE, +SIZE, +SIZE);
    const glm::vec3 V7 = glm::vec3(+SIZE, +SIZE, +SIZE);

    // normals
    const glm::vec3 N0 = glm::vec3(+0.f, +0.f, -1.f);
    const glm::vec3 N1 = glm::vec3(+1.f, +0.f, +0.f);
    const glm::vec3 N2 = glm::vec3(+0.f, +0.f, +1.f);
    const glm::vec3 N3 = glm::vec3(-1.f, +0.f, +0.f);
    const glm::vec3 N4 = glm::vec3(+0.f, +1.f, +0.f);
    const glm::vec3 N5 = glm::vec3(+0.f, -1.f, +0.f);

    const float F13 = 1.f / 3.f;
    const float F23 = 2.f / 3.f;

    // (0) -z
    uint32_t i = 0;
    geom_vertices[i * 4 + 0] = V0;
    geom_vertices[i * 4 + 1] = V1;
    geom_vertices[i * 4 + 2] = V2;
    geom_vertices[i * 4 + 3] = V3;
    tex_vertices[i * 4 + 0] = glm::vec2(1.f, F13);
    tex_vertices[i * 4 + 1] = glm::vec2(.75f, F13);
    tex_vertices[i * 4 + 2] = glm::vec2(1.f, F23);
    tex_vertices[i * 4 + 3] = glm::vec2(.75f, F23);
    norm_vertices[i * 4 + 0] = N0;
    norm_vertices[i * 4 + 1] = N0;
    norm_vertices[i * 4 + 2] = N0;
    norm_vertices[i * 4 + 3] = N0;
    indices[i * 6 + 0] = i * 4 + 0;
    indices[i * 6 + 1] = i * 4 + 3;
    indices[i * 6 + 2] = i * 4 + 1;
    indices[i * 6 + 3] = i * 4 + 0;
    indices[i * 6 + 4] = i * 4 + 2;
    indices[i * 6 + 5] = i * 4 + 3;

    // (1) +x
    i++;
    geom_vertices[i * 4 + 0] = V7;
    geom_vertices[i * 4 + 1] = V1;
    geom_vertices[i * 4 + 2] = V5;
    geom_vertices[i * 4 + 3] = V3;
    tex_vertices[i * 4 + 0] = glm::vec2(.5f, F23);
    tex_vertices[i * 4 + 1] = glm::vec2(.75f, F13);
    tex_vertices[i * 4 + 2] = glm::vec2(.5f, F13);
    tex_vertices[i * 4 + 3] = glm::vec2(.75f, F23);
    norm_vertices[i * 4 + 0] = N1;
    norm_vertices[i * 4 + 1] = N1;
    norm_vertices[i * 4 + 2] = N1;
    norm_vertices[i * 4 + 3] = N1;
    indices[i * 6 + 0] = i * 4 + 1;
    indices[i * 6 + 1] = i * 4 + 0;
    indices[i * 6 + 2] = i * 4 + 2;
    indices[i * 6 + 3] = i * 4 + 1;
    indices[i * 6 + 4] = i * 4 + 3;
    indices[i * 6 + 5] = i * 4 + 0;

    // (2) +z
    i++;
    geom_vertices[i * 4 + 0] = V7;
    geom_vertices[i * 4 + 1] = V6;
    geom_vertices[i * 4 + 2] = V5;
    geom_vertices[i * 4 + 3] = V4;
    tex_vertices[i * 4 + 0] = glm::vec2(.5f, F23);
    tex_vertices[i * 4 + 1] = glm::vec2(.25f, F23);
    tex_vertices[i * 4 + 2] = glm::vec2(.5f, F13);
    tex_vertices[i * 4 + 3] = glm::vec2(.25f, F13);
    norm_vertices[i * 4 + 0] = N2;
    norm_vertices[i * 4 + 1] = N2;
    norm_vertices[i * 4 + 2] = N2;
    norm_vertices[i * 4 + 3] = N2;
    indices[i * 6 + 0] = i * 4 + 2;
    indices[i * 6 + 1] = i * 4 + 1;
    indices[i * 6 + 2] = i * 4 + 3;
    indices[i * 6 + 3] = i * 4 + 2;
    indices[i * 6 + 4] = i * 4 + 0;
    indices[i * 6 + 5] = i * 4 + 1;

    // (3) -x
    i++;
    geom_vertices[i * 4 + 0] = V0;
    geom_vertices[i * 4 + 1] = V6;
    geom_vertices[i * 4 + 2] = V2;
    geom_vertices[i * 4 + 3] = V4;
    tex_vertices[i * 4 + 0] = glm::vec2(0.f, F13);
    tex_vertices[i * 4 + 1] = glm::vec2(.25f, F23);
    tex_vertices[i * 4 + 2] = glm::vec2(0.f, F23);
    tex_vertices[i * 4 + 3] = glm::vec2(.25f, F13);
    norm_vertices[i * 4 + 0] = N3;
    norm_vertices[i * 4 + 1] = N3;
    norm_vertices[i * 4 + 2] = N3;
    norm_vertices[i * 4 + 3] = N3;
    indices[i * 6 + 0] = i * 4 + 3;
    indices[i * 6 + 1] = i * 4 + 2;
    indices[i * 6 + 2] = i * 4 + 0;
    indices[i * 6 + 3] = i * 4 + 3;
    indices[i * 6 + 4] = i * 4 + 1;
    indices[i * 6 + 5] = i * 4 + 2;

    // (4) +y
    i++;
    geom_vertices[i * 4 + 0] = V7;
    geom_vertices[i * 4 + 1] = V6;
    geom_vertices[i * 4 + 2] = V2;
    geom_vertices[i * 4 + 3] = V3;
    tex_vertices[i * 4 + 0] = glm::vec2(.5f, F23);
    tex_vertices[i * 4 + 1] = glm::vec2(.25f, F23);
    tex_vertices[i * 4 + 2] = glm::vec2(.25f, 1.f);
    tex_vertices[i * 4 + 3] = glm::vec2(.5f, 1.f);
    norm_vertices[i * 4 + 0] = N4;
    norm_vertices[i * 4 + 1] = N4;
    norm_vertices[i * 4 + 2] = N4;
    norm_vertices[i * 4 + 3] = N4;
    indices[i * 6 + 0] = i * 4 + 0;
    indices[i * 6 + 1] = i * 4 + 2;
    indices[i * 6 + 2] = i * 4 + 1;
    indices[i * 6 + 3] = i * 4 + 0;
    indices[i * 6 + 4] = i * 4 + 3;
    indices[i * 6 + 5] = i * 4 + 2;

    // (5) -y
    i++;
    geom_vertices[i * 4 + 0] = V0;
    geom_vertices[i * 4 + 1] = V1;
    geom_vertices[i * 4 + 2] = V5;
    geom_vertices[i * 4 + 3] = V4;
    tex_vertices[i * 4 + 0] = glm::vec2(.25f, 0.f);
    tex_vertices[i * 4 + 1] = glm::vec2(.5f, 0.f);
    tex_vertices[i * 4 + 2] = glm::vec2(.5f, F13);
    tex_vertices[i * 4 + 3] = glm::vec2(.25f, F13);
    norm_vertices[i * 4 + 0] = N5;
    norm_vertices[i * 4 + 1] = N5;
    norm_vertices[i * 4 + 2] = N5;
    norm_vertices[i * 4 + 3] = N5;
    indices[i * 6 + 0] = i * 4 + 2;
    indices[i * 6 + 1] = i * 4 + 3;
    indices[i * 6 + 2] = i * 4 + 0;
    indices[i * 6 + 3] = i * 4 + 2;
    indices[i * 6 + 4] = i * 4 + 0;
    indices[i * 6 + 5] = i * 4 + 1;

    // todo tangent and bitangent
    create_mesh(mesh, geom_vertices, tex_vertices, norm_vertices,
                norm_vertices, norm_vertices, vertex_count,
                indices, index_count
    );
}

void Mesh::create_skybox(Mesh *mesh)
{
    // NB: basically {create_cube} with different texture coordinates and differently ordered indices
    const float SIZE = 1.f;

    const uint32_t vertex_count = 6 * 4;
    glm::vec3 geom_vertices[vertex_count];
    glm::vec2 tex_vertices[vertex_count];
    glm::vec3 norm_vertices[vertex_count];
    const uint32_t index_count = 6 * 6;
    GLushort indices[index_count];

    const glm::vec3 V0 = glm::vec3(-SIZE, -SIZE, -SIZE);
    const glm::vec3 V1 = glm::vec3(+SIZE, -SIZE, -SIZE);
    const glm::vec3 V2 = glm::vec3(-SIZE, +SIZE, -SIZE);
    const glm::vec3 V3 = glm::vec3(+SIZE, +SIZE, -SIZE);
    const glm::vec3 V4 = glm::vec3(-SIZE, -SIZE, +SIZE);
    const glm::vec3 V5 = glm::vec3(+SIZE, -SIZE, +SIZE);
    const glm::vec3 V6 = glm::vec3(-SIZE, +SIZE, +SIZE);
    const glm::vec3 V7 = glm::vec3(+SIZE, +SIZE, +SIZE);

    const glm::vec3 N0 = glm::vec3(+0.f, +0.f, -1.f);
    const glm::vec3 N1 = glm::vec3(+1.f, +0.f, +0.f);
    const glm::vec3 N2 = glm::vec3(+0.f, +0.f, +1.f);
    const glm::vec3 N3 = glm::vec3(-1.f, +0.f, +0.f);
    const glm::vec3 N4 = glm::vec3(+0.f, +1.f, +0.f);
    const glm::vec3 N5 = glm::vec3(+0.f, -1.f, +0.f);

    const float F13 = 1.f / 3.f;
    const float F23 = 2.f / 3.f;

    // (0) -z
    uint32_t i = 0;
    geom_vertices[i * 4 + 0] = V0;
    geom_vertices[i * 4 + 1] = V1;
    geom_vertices[i * 4 + 2] = V2;
    geom_vertices[i * 4 + 3] = V3;
    tex_vertices[i * 4 + 0] = glm::vec2(1.f, F13);
    tex_vertices[i * 4 + 1] = glm::vec2(.75f, F13);
    tex_vertices[i * 4 + 2] = glm::vec2(1.f, F23);
    tex_vertices[i * 4 + 3] = glm::vec2(.75f, F23);
    norm_vertices[i * 4 + 0] = N0;
    norm_vertices[i * 4 + 1] = N0;
    norm_vertices[i * 4 + 2] = N0;
    norm_vertices[i * 4 + 3] = N0;
    indices[i * 6 + 0] = i * 4 + 0;
    indices[i * 6 + 1] = i * 4 + 1;
    indices[i * 6 + 2] = i * 4 + 3;
    indices[i * 6 + 3] = i * 4 + 0;
    indices[i * 6 + 4] = i * 4 + 3;
    indices[i * 6 + 5] = i * 4 + 2;

    // (1) +x
    i++;
    geom_vertices[i * 4 + 0] = V7;
    geom_vertices[i * 4 + 1] = V1;
    geom_vertices[i * 4 + 2] = V5;
    geom_vertices[i * 4 + 3] = V3;
    tex_vertices[i * 4 + 0] = glm::vec2(.5f, F23);
    tex_vertices[i * 4 + 1] = glm::vec2(.75f, F13);
    tex_vertices[i * 4 + 2] = glm::vec2(.5f, F13);
    tex_vertices[i * 4 + 3] = glm::vec2(.75f, F23);
    norm_vertices[i * 4 + 0] = N1;
    norm_vertices[i * 4 + 1] = N1;
    norm_vertices[i * 4 + 2] = N1;
    norm_vertices[i * 4 + 3] = N1;
    indices[i * 6 + 0] = i * 4 + 1;
    indices[i * 6 + 1] = i * 4 + 2;
    indices[i * 6 + 2] = i * 4 + 0;
    indices[i * 6 + 3] = i * 4 + 1;
    indices[i * 6 + 4] = i * 4 + 0;
    indices[i * 6 + 5] = i * 4 + 3;

    // (2) +z
    i++;
    geom_vertices[i * 4 + 0] = V7;
    geom_vertices[i * 4 + 1] = V6;
    geom_vertices[i * 4 + 2] = V5;
    geom_vertices[i * 4 + 3] = V4;
    tex_vertices[i * 4 + 0] = glm::vec2(.5f, F23);
    tex_vertices[i * 4 + 1] = glm::vec2(.25f, F23);
    tex_vertices[i * 4 + 2] = glm::vec2(.5f, F13);
    tex_vertices[i * 4 + 3] = glm::vec2(.25f, F13);
    norm_vertices[i * 4 + 0] = N2;
    norm_vertices[i * 4 + 1] = N2;
    norm_vertices[i * 4 + 2] = N2;
    norm_vertices[i * 4 + 3] = N2;
    indices[i * 6 + 0] = i * 4 + 2;
    indices[i * 6 + 1] = i * 4 + 3;
    indices[i * 6 + 2] = i * 4 + 1;
    indices[i * 6 + 3] = i * 4 + 2;
    indices[i * 6 + 4] = i * 4 + 1;
    indices[i * 6 + 5] = i * 4 + 0;

    // (3) -x
    i++;
    geom_vertices[i * 4 + 0] = V0;
    geom_vertices[i * 4 + 1] = V6;
    geom_vertices[i * 4 + 2] = V2;
    geom_vertices[i * 4 + 3] = V4;
    tex_vertices[i * 4 + 0] = glm::vec2(0.f, F13);
    tex_vertices[i * 4 + 1] = glm::vec2(.25f, F23);
    tex_vertices[i * 4 + 2] = glm::vec2(0.f, F23);
    tex_vertices[i * 4 + 3] = glm::vec2(.25f, F13);
    norm_vertices[i * 4 + 0] = N3;
    norm_vertices[i * 4 + 1] = N3;
    norm_vertices[i * 4 + 2] = N3;
    norm_vertices[i * 4 + 3] = N3;
    indices[i * 6 + 0] = i * 4 + 3;
    indices[i * 6 + 1] = i * 4 + 0;
    indices[i * 6 + 2] = i * 4 + 2;
    indices[i * 6 + 3] = i * 4 + 3;
    indices[i * 6 + 4] = i * 4 + 2;
    indices[i * 6 + 5] = i * 4 + 1;

    // (4) +y
    i++;
    geom_vertices[i * 4 + 0] = V7;
    geom_vertices[i * 4 + 1] = V6;
    geom_vertices[i * 4 + 2] = V2;
    geom_vertices[i * 4 + 3] = V3;
    tex_vertices[i * 4 + 0] = glm::vec2(.5f, F23);
    tex_vertices[i * 4 + 1] = glm::vec2(.25f, F23);
    tex_vertices[i * 4 + 2] = glm::vec2(.25f, 1.f);
    tex_vertices[i * 4 + 3] = glm::vec2(.5f, 1.f);
    norm_vertices[i * 4 + 0] = N4;
    norm_vertices[i * 4 + 1] = N4;
    norm_vertices[i * 4 + 2] = N4;
    norm_vertices[i * 4 + 3] = N4;
    indices[i * 6 + 0] = i * 4 + 0;
    indices[i * 6 + 1] = i * 4 + 1;
    indices[i * 6 + 2] = i * 4 + 2;
    indices[i * 6 + 3] = i * 4 + 0;
    indices[i * 6 + 4] = i * 4 + 2;
    indices[i * 6 + 5] = i * 4 + 3;

    // (5) -y
    i++;
    geom_vertices[i * 4 + 0] = V0;
    geom_vertices[i * 4 + 1] = V1;
    geom_vertices[i * 4 + 2] = V5;
    geom_vertices[i * 4 + 3] = V4;
    tex_vertices[i * 4 + 0] = glm::vec2(.25f, 0.f);
    tex_vertices[i * 4 + 1] = glm::vec2(.5f, 0.f);
    tex_vertices[i * 4 + 2] = glm::vec2(.5f, F13);
    tex_vertices[i * 4 + 3] = glm::vec2(.25f, F13);
    norm_vertices[i * 4 + 0] = N5;
    norm_vertices[i * 4 + 1] = N5;
    norm_vertices[i * 4 + 2] = N5;
    norm_vertices[i * 4 + 3] = N5;
    indices[i * 6 + 0] = i * 4 + 2;
    indices[i * 6 + 1] = i * 4 + 0;
    indices[i * 6 + 2] = i * 4 + 3;
    indices[i * 6 + 3] = i * 4 + 2;
    indices[i * 6 + 4] = i * 4 + 1;
    indices[i * 6 + 5] = i * 4 + 0;

    // todo tangent and bitangent
    create_mesh(mesh,
                geom_vertices, tex_vertices, norm_vertices,
                norm_vertices, norm_vertices, vertex_count,
                indices, index_count);
}

void Mesh::create_sphere(Mesh *mesh)
{
    const uint32_t STACK_COUNT = 64;
    const uint32_t SECTOR_COUNT = 64;
    const uint32_t HOR_TEXTURE_SCALE = 3;
    const uint32_t VER_TEXTURE_SCALE = 2;

    /** create vertices */

    const uint32_t VERTEX_COUNT = (STACK_COUNT + 1) * (SECTOR_COUNT + 1);
    glm::vec3 geom_vertices[VERTEX_COUNT];
    glm::vec2 tex_vertices[VERTEX_COUNT];
    glm::vec3 norm_vertices[VERTEX_COUNT];
    glm::vec3 tangent_vertices[VERTEX_COUNT];
    glm::vec3 bitangent_vertices[VERTEX_COUNT];

    const float STACK_STEP = M_PI / (float) STACK_COUNT;
    const float SECTOR_STEP = (2.f * (float) M_PI) / (float) SECTOR_COUNT;

    uint32_t vertex = 0; // vertex index
    for (uint32_t i = 0; i < STACK_COUNT + 1; i++) {
        float stack_angle = (float) M_PI_2 - (float) i * STACK_STEP; // phi from pi / 2 to -pi / 2
        float xz = cosf(stack_angle);                                // cos(phi)
        float y = sinf(stack_angle);                                 // sin(phi)

        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        for (uint32_t j = 0; j < SECTOR_COUNT + 1; j++) {
            float sector_angle = (float) j * SECTOR_STEP; // theta from 0 to 2 * pi

            // vertex position (x, y, z)
            float cos_theta = cosf(sector_angle);         // cos(theta)
            float sin_theta = sinf(sector_angle);         // sin(theta)
            float x = xz * cos_theta;                     // cos(phi) * cos(theta)
            float z = xz * sin_theta;                     // cos(phi) * sin(theta)
            geom_vertices[vertex] = glm::vec3(x, y, z);

            // normalized vertex normal (nx, ny, nz)
            norm_vertices[vertex] = glm::vec3(x, y, z);

            // vertex tex coord (s, t) range between [0, 1]
            float s = 1.f - (float) j / (float) (SECTOR_COUNT / (float) HOR_TEXTURE_SCALE);
            float t = 1.f - (float) i / (float) (STACK_COUNT / (float) VER_TEXTURE_SCALE);
            tex_vertices[vertex] = glm::vec2(s, t);

            // tangent: (unitized) derivative w.r.t. theta (https://computergraphics.stackexchange.com/a/5499)
            tangent_vertices[vertex] = glm::vec3(-sin_theta, 0.f, cos_theta);

            // bitangent: (unitized) derivative w.r.t. phi (a little bit uglier)
            bitangent_vertices[vertex] = glm::normalize(glm::vec3(-y * cos_theta, xz, -y * sin_theta));

            vertex++;
        }
    }

    /** create indices */

    const uint32_t INDEX_COUNT = STACK_COUNT * (SECTOR_COUNT - 2) * 6;
    GLushort indices[INDEX_COUNT];

    uint32_t index = 0; // index index
    for (uint32_t i = 0; i < STACK_COUNT; i++) {
        uint32_t k1 = i * (SECTOR_COUNT + 1); // beginning of current stack
        uint32_t k2 = k1 + SECTOR_COUNT + 1;  // beginning of next stack

        for (uint32_t j = 0; j < SECTOR_COUNT; j++) {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k1+1 => k2
            if (i != 0) {
                indices[index++] = k1;
                indices[index++] = k1 + 1;
                indices[index++] = k2;
            }

            // k1+1 => k2 + 1 => k2
            if (i != (STACK_COUNT - 1)) {
                indices[index++] = k1 + 1;
                indices[index++] = k2 + 1;
                indices[index++] = k2;
            }

            k1++;
            k2++;
        }
    }

    create_mesh(mesh, geom_vertices, tex_vertices, norm_vertices,
                tangent_vertices, bitangent_vertices, VERTEX_COUNT,
                indices, INDEX_COUNT);
}
