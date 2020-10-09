#ifndef PBR_LIGHT_HPP
#define PBR_LIGHT_HPP

#include <glm/vec3.hpp>

#include "scene_object.hpp"
#include "../system/camera.hpp"
#include "../system/shader_manager.hpp"
#include "../system/texture_manager.hpp"
#include "../system/mesh_manager.hpp"

class Light : public SceneObject {
private:
    const float SCALE = 10.f;
public:
    glm::vec3 color;

    Light(Scene *scene, glm::vec3 position, glm::vec3 color);

    void render(
            Camera *camera, ShaderManager *shader_manager, TextureManager *texture_manager, MeshManager *mesh_manager,
            bool debug_mode) override;

    bool hit(float *t, glm::vec3 origin, glm::vec3 direction) override;
};

#endif //PBR_LIGHT_HPP
