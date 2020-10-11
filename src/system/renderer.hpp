#ifndef SYSTEM_RENDERER_HPP
#define SYSTEM_RENDERER_HPP

#include <vector>
#include <cstdint>

#include <glm/gtc/matrix_transform.hpp>

#include "camera.hpp"
#include "manager/shader_manager.hpp"
#include "manager/texture_manager.hpp"
#include "manager/primitive_manager.hpp"
#include "../scene/scene.hpp"

class Renderer {
private:
    Camera *camera;
    ShaderManager *shader_manager;
    TextureManager *texture_manager;
    PrimitiveManager *primitive_manager;

    /** Whether the coordinate system should be drawn. */
    bool debug_mode = false;
public:

    Renderer(
            Camera *p_camera, ShaderManager *p_shader_manager, TextureManager *p_texture_manager,
            PrimitiveManager *p_primitive_manager
    );

    ~Renderer() = default;

    void render(Scene *scene);

    void toggle_draw_coordinate();

    void render_pbr(
            uint32_t mesh_id, uint32_t material_id, std::vector<glm::vec3> positions,
            std::vector<glm::vec3> colors, glm::mat4 model_matrix);

    void render_default(uint32_t mesh_id, glm::vec3 color, glm::mat4 model_matrix);

    void render_lines(uint32_t primitive_id, glm::mat4 model_matrix);
};


#endif //SYSTEM_RENDERER_HPP
