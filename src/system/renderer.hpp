#ifndef SYSTEM_RENDERER_HPP
#define SYSTEM_RENDERER_HPP

#include <vector>
#include <cstdint>

#include <glm/gtc/matrix_transform.hpp>

#include "camera.hpp"
#include "opengl.hpp"
#include "shader_manager.hpp"
#include "texture_manager.hpp"
#include "mesh_manager.hpp"

class Renderer {
private:
    Camera *camera;
    ShaderManager *shader_manager;
    TextureManager *texture_manager;
    MeshManager *mesh_manager;

    /** Whether the coordinate system should be drawn. */
    bool draw_coordinate = false;

    /** Contains the mesh for the coordinate system. */
    Lines coordinate_mesh{};

    Lines normals{}; // todo debug
public:

    Renderer(
            Camera *p_camera, ShaderManager *p_shader_manager, TextureManager *p_texture_manager,
            MeshManager *p_mesh_manager
    );

    ~Renderer();

    void render();

    void toggle_draw_coordinate();
};


#endif //SYSTEM_RENDERER_HPP
