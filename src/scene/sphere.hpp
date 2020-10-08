#ifndef PBR_SPHERE_HPP
#define PBR_SPHERE_HPP

#include "scene_object.hpp"

class Sphere : public SceneObject {
private:
    Lines normals{}; // todo debug
public:
    Sphere(Scene *scene, const glm::vec3 &position);

    void render(
            Camera *camera, ShaderManager *shader_manager, TextureManager *texture_manager, MeshManager *mesh_manager,
            bool debug_mode) override;
};

#endif //PBR_SPHERE_HPP
