#ifndef SCENE_SCENE_HPP
#define SCENE_SCENE_HPP

#include <vector>

#include "light.hpp"
#include "../system/camera.hpp"
#include "../system/manager/shader_manager.hpp"
#include "../system/manager/texture_manager.hpp"
#include "../system/manager/primitive_manager.hpp"

class SceneObject;

class Light;

class Scene {
private:
    Renderer *renderer;
public:
    std::vector<SceneObject *> objects;
    std::vector<Light *> lights;
    /** Whether a scene object is selected. Redundant information since scene objects also maintain this,
     * but the alternative is a pointer to the selected object, which may go invalid.*/
    bool has_selection = false;

    bool scene_one = true;

    explicit Scene(Renderer *renderer);

    virtual ~Scene();

    void erase();

    /** Scene with four spheres. */
    void construct();

    /** Scene with one high detail sphere and lights. */
    void construct_other();

    void switch_scene();

    void render(bool debug_mode);

    void update();

    void cast_ray(glm::vec3 origin, glm::vec3 direction);
};

#endif //SCENE_SCENE_HPP
