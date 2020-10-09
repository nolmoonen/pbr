#ifndef PBR_SPHERE_HPP
#define PBR_SPHERE_HPP

#include "scene_object.hpp"
#include "../system/opengl/lines.hpp"
#include "scene.hpp"
#include "../system/camera.hpp"
#include "../system/shader_manager.hpp"
#include "../system/texture_manager.hpp"
#include "../system/mesh_manager.hpp"

class Sphere : public SceneObject {
private:
    Lines normals{}; // todo debug
public:
    Sphere(Scene *scene, const glm::vec3 &position);

    void render(
            Camera *camera, ShaderManager *shader_manager, TextureManager *texture_manager, MeshManager *mesh_manager,
            bool debug_mode) override;

    bool hit(float *t, glm::vec3 origin, glm::vec3 direction) override;
};

#endif //PBR_SPHERE_HPP
