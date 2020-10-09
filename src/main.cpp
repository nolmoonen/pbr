#include <cstdlib>

#include <glm/mat4x4.hpp>

#include "system/window.hpp"
#include "system/camera.hpp"
#include "system/renderer.hpp"
#include "system/texture_manager.hpp"

void update(Camera *camera, Renderer *renderer, Scene *scene);

int main()
{
    if (window::get_instance().initialize() == EXIT_FAILURE) {
        nm_log::log(LOG_ERROR, "failed to create window\n");
        return EXIT_FAILURE;
    }

    ShaderManager shader_manager;
    TextureManager texture_manager;
    MeshManager mesh_manager;

    Camera camera(
            (float) window::get_instance().get_input_handler()->get_size_x() /
            (float) window::get_instance().get_input_handler()->get_size_y(),
            glm::radians(90.f), .1f, 1000.f);

    Renderer renderer(&camera, &shader_manager, &texture_manager, &mesh_manager);

    Scene scene;

    while (!window::get_instance().should_close()) {
        window::get_instance().get_input_handler()->pull_input();

        // process system input
        update(&camera, &renderer, &scene);

        scene.update();

        renderer.render(&scene);

        // allow managers to deallocate objects not used in last frame
        shader_manager.make_space();
        texture_manager.make_space();
        mesh_manager.make_space();
    }

    window::get_instance().cleanup();

    return EXIT_SUCCESS;
}

void update(Camera *camera, Renderer *renderer, Scene *scene)
{
    // if ESC is pressed, close the window
    if (window::get_instance().get_input_handler()->get_key_state(input::ESCAPE, input::PRESSED)) {
        window::get_instance().set_to_close();
    }

    // update camera zoom
    camera->add_zoom((float) window::get_instance().get_input_handler()->get_yoffset());

    // update camera position
    if (window::get_instance().get_input_handler()->get_mouse_button_state(input::RMB, input::DOWN)) {
        camera->translate(
                window::get_instance().get_input_handler()->get_offset_xpos(),
                window::get_instance().get_input_handler()->get_offset_ypos()
        );
    }

    // update camera rotation
    if (window::get_instance().get_input_handler()->get_mouse_button_state(input::MMB, input::DOWN)) {
        camera->rotate(
                window::get_instance().get_input_handler()->get_offset_xpos(),
                window::get_instance().get_input_handler()->get_offset_ypos()
        );
    }

    if (window::get_instance().get_input_handler()->is_resized()) {
        uint32_t size_x = window::get_instance().get_input_handler()->get_size_x();
        uint32_t size_y = window::get_instance().get_input_handler()->get_size_y();

        // update camera aspect ratio
        camera->set_aspect((float) size_x / (float) size_y);

        // update gl viewport
        glViewport(0, 0, size_x, size_y);
    }

    // if D is pressed, draw the coordinate system
    if (window::get_instance().get_input_handler()->get_key_state(input::D, input::PRESSED)) {
        renderer->toggle_draw_coordinate();
    }

    // if LMB is pressed, cast a ray in the scene to find intersecting objects
    if (window::get_instance().get_input_handler()->get_mouse_button_state(input::LMB, input::PRESSED)) {
        double x = (window::get_instance().get_input_handler()->get_xpos() /
                    (double) window::get_instance().get_input_handler()->get_size_x()) * 2.f - 1.f;
        double y = 1.f - (window::get_instance().get_input_handler()->get_ypos() /
                          (double) window::get_instance().get_input_handler()->get_size_y()) * 2.f;

        // todo could manually define these matrices to not use general purpose inverse function
        glm::mat4 inv_projection = glm::inverse(camera->get_proj_matrix());
        glm::mat4 inv_view = glm::inverse(camera->get_view_matrix());

        glm::mat4 inv_proj_view = inv_view * inv_projection;

        glm::vec4 near = inv_proj_view * glm::vec4(x, y, -1.f, 1.f);
        glm::vec4 far = inv_proj_view * glm::vec4(x, y, 1.f, 1.f);

        glm::vec3 a = glm::vec3(near.x / near.w, near.y / near.w, near.z / near.w);
        glm::vec3 b = glm::vec3(far.x / far.w, far.y / far.w, far.z / far.w);
        glm::vec3 dir = glm::normalize(b - a);

        scene->cast_ray(a, dir);
    }
}
