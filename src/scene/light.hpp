#ifndef PBR_LIGHT_HPP
#define PBR_LIGHT_HPP

#include <glm/vec3.hpp>
#include "../system/camera.hpp"
#include "../system/shader_manager.hpp"
#include "../system/texture_manager.hpp"
#include "../system/mesh_manager.hpp"

class Light {
public:
    glm::vec3 position;
    glm::vec3 color;

    Light(glm::vec3 position, glm::vec3 color);

    void render(Camera *camera, ShaderManager *shader_manager, TextureManager *texture_manager,
                MeshManager *mesh_manager) const;
};

#endif //PBR_LIGHT_HPP
