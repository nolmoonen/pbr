#ifndef SYSTEM_RENDERER_HPP
#define SYSTEM_RENDERER_HPP

#include <vector>
#include <cstdint>

#include <glm/gtc/matrix_transform.hpp>

#include "camera.hpp"
#include "shader_manager.hpp"
#include "texture_manager.hpp"
#include "mesh_manager.hpp"
#include "../scene/scene.hpp"
#include "opengl/lines.hpp"

class Renderer {
private:
    Camera *camera;
    ShaderManager *shader_manager;
    TextureManager *texture_manager;
    MeshManager *mesh_manager;

    /** Whether the coordinate system should be drawn. */
    bool debug_mode = false;

    /** Contains the mesh for the coordinate system. */
    Lines coordinate_mesh{};
public:

    Renderer(
            Camera *p_camera, ShaderManager *p_shader_manager, TextureManager *p_texture_manager,
            MeshManager *p_mesh_manager
    );

    ~Renderer();

    void render(Scene *scene);

    void toggle_draw_coordinate();

    void render_coordinates(glm::mat4 model_matrix);

    void render_pbr(
            uint32_t mesh_id, uint32_t material_id, std::vector<glm::vec3> positions,
            std::vector<glm::vec3> colors, glm::mat4 model_matrix);

    void render_default(uint32_t mesh_id, glm::mat4 model_matrix);

    // todo ugly way to pass in lines struct
    void render_lines(Lines *lines, glm::mat4 model_matrix);
};


#endif //SYSTEM_RENDERER_HPP
