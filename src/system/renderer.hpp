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

    TextureType cubemap = CUBEMAP_CAYLEY_INTERIOR;
    TextureType cubemap_irradiance = CUBEMAP_CAYLEY_INTERIOR_IRRADIANCE;

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

    void switch_skybox(TextureType p_cubemap, TextureType p_cubemap_irradiance);

private:
    const float WIDGET_CONE_BASE_RADIUS = .03f;
    // since the cone is uniformly scaled and the cone is 2x2x2
    const float WIDGET_CONE_HEIGHT = WIDGET_CONE_BASE_RADIUS * 2.f; // do not change
    const float CYLINDER_LENGTH = .3f;
    const float CYLINDER_RADIUS = .02f;
public:
    void render_widget(glm::vec3 position);

    /**
     * Getters for intersection testing.
     */
    float get_scale(glm::vec3 position);

    float get_widget_cone_base_radius(glm::vec3 position);

    float get_widget_cone_height(glm::vec3 position);

    float get_cylinder_length(glm::vec3 position);

    float get_cylinder_radius(glm::vec3 position);

    void render_skybox();
};


#endif //SYSTEM_RENDERER_HPP
