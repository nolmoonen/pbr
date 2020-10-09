#include <cstdlib>

#include <glm/mat4x4.hpp>

#include "system/window.hpp"
#include "system/camera.hpp"
#include "system/renderer.hpp"
#include "system/texture_manager.hpp"

void update(Camera *camera, Renderer *renderer);

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
        update(&camera, &renderer);

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

void update(Camera *camera, Renderer *renderer)
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
}
