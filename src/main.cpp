#include <cstdlib>

#include <glm/mat4x4.hpp>

#include "system/window.hpp"
#include "system/camera.hpp"
#include "system/renderer.hpp"
#include "system/manager/texture_manager.hpp"
#include "util/nm_math.hpp"

void update(Camera *camera, Renderer *renderer, Scene *scene);

int main()
{
    if (window::get_instance().initialize() == EXIT_FAILURE) {
        nm_log::log(LOG_ERROR, "failed to create window\n");
        return EXIT_FAILURE;
    }

    ShaderManager shader_manager;
    TextureManager texture_manager;
    PrimitiveManager primitive_manager;

    Camera camera(
            (float) window::get_instance().get_input_handler()->get_size_x() /
            (float) window::get_instance().get_input_handler()->get_size_y(),
            glm::radians(90.f), .1f, 1000.f);

    Renderer renderer(&camera, &shader_manager, &texture_manager, &primitive_manager);

    Scene scene(&renderer);

    while (!window::get_instance().should_close()) {
        window::get_instance().get_input_handler()->pull_input();

        // process system input
        update(&camera, &renderer, &scene);

        scene.update();

        renderer.render(&scene);

        // allow managers to deallocate objects not used in last frame
        shader_manager.make_space();
        texture_manager.make_space();
        primitive_manager.make_space();
    }

    window::get_instance().cleanup();

    return EXIT_SUCCESS;
}

void update(Camera *camera, Renderer *renderer, Scene *scene)
{
    // state variables for dragging scene objects
    static bool dragging_widget = false; // whether we are currently dragging the widget
    static uint32_t dragging_axis = 0;   // axis of the widget 0 = x, 1 = y, 2 = z

    // if ESC is pressed, close the window
    if (window::get_instance().get_input_handler()->get_key_state(input::ESCAPE, input::PRESSED)) {
        window::get_instance().set_to_close();
    }

    if (window::get_instance().get_input_handler()->get_key_state(input::NUM_1, input::PRESSED)) {
        renderer->switch_skybox(CUBEMAP_CAYLEY_INTERIOR, CUBEMAP_CAYLEY_INTERIOR_IRRADIANCE);
    }

    if (window::get_instance().get_input_handler()->get_key_state(input::NUM_2, input::PRESSED)) {
        renderer->switch_skybox(CUBEMAP_STUDIO, CUBEMAP_STUDIO_IRRADIANCE);
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

        // if the scene has an object selected, first see if the widget is hit
        float tx1, tx2, ty1, ty2, tz1, tz2;
        bool x1, x2, y1, y2, z1, z2;
        if (scene->has_selection) {
            glm::vec3 widget_pos;
            for (auto &object : scene->objects) {
                if (object->selected) {
                    widget_pos = object->position;
                    break;
                }
            }

            const glm::vec3 X = glm::vec3(1.f, 0.f, 0.f);
            x1 = nm_math::ray_cylinder(
                    &tx1, a, dir, widget_pos, widget_pos + renderer->get_cylinder_length(widget_pos) * X,
                    renderer->get_cylinder_radius(widget_pos));
            x2 = nm_math::ray_cone(
                    &tx2, a, dir, widget_pos + (renderer->get_cylinder_length(widget_pos) +
                                                renderer->get_widget_cone_height(widget_pos)) * X, -X,
                    renderer->get_widget_cone_height(widget_pos), renderer->get_widget_cone_base_radius(widget_pos));
            const glm::vec3 Y = glm::vec3(0.f, 1.f, 0.f);
            y1 = nm_math::ray_cylinder(
                    &ty1, a, dir, widget_pos, widget_pos + renderer->get_cylinder_length(widget_pos) * Y,
                    renderer->get_cylinder_radius(widget_pos));
            y2 = nm_math::ray_cone(
                    &ty2, a, dir, widget_pos + (renderer->get_cylinder_length(widget_pos) +
                                                renderer->get_widget_cone_height(widget_pos)) * Y, -Y,
                    renderer->get_widget_cone_height(widget_pos), renderer->get_widget_cone_base_radius(widget_pos));
            const glm::vec3 Z = glm::vec3(0.f, 0.f, 1.f);
            z1 = nm_math::ray_cylinder(
                    &tz1, a, dir, widget_pos, widget_pos + renderer->get_cylinder_length(widget_pos) * Z,
                    renderer->get_cylinder_radius(widget_pos));
            z2 = nm_math::ray_cone(
                    &tz2, a, dir, widget_pos + (renderer->get_cylinder_length(widget_pos) +
                                                renderer->get_widget_cone_height(widget_pos)) * Z, -Z,
                    renderer->get_widget_cone_height(widget_pos), renderer->get_widget_cone_base_radius(widget_pos));
        }

        if (scene->has_selection && (x1 || x2 || y1 || y2 || z1 || z2)) {
            dragging_widget = true;

            float t = std::numeric_limits<float>::max();
            if (x1 && tx1 < t) dragging_axis = 0;
            if (x2 && tx2 < t) dragging_axis = 0;
            if (y1 && ty1 < t) dragging_axis = 1;
            if (y2 && ty2 < t) dragging_axis = 1;
            if (z1 && tz1 < t) dragging_axis = 2;
            if (z2 && tz2 < t) dragging_axis = 2;
        } else {
            // only continue with scene objects if the widget was not hit
            scene->cast_ray(a, dir);
        }
    }

    if (dragging_widget) {
        const float SENSITIVITY = .08f;
        double x_offset = window::get_instance().get_input_handler()->get_offset_xpos();
        double y_offset = window::get_instance().get_input_handler()->get_offset_ypos();
        // only proceed if there is a change in mouse position
        if (x_offset != 0. || y_offset != 0.) {
            glm::vec3 axis;
            switch (dragging_axis) {
                case 0:
                    axis = glm::vec3(1.f, 0.f, 0.f);
                    break;
                case 1:
                    axis = glm::vec3(0.f, 1.f, 0.f);
                    break;
                case 2:
                default:
                    axis = glm::vec3(0.f, 0.f, 1.f);
            }

            // find scene object that is selected
            SceneObject *object;
            for (auto &obj : scene->objects) {
                if (obj->selected) {
                    object = obj;
                    break;
                }
            }

            double new_x = (window::get_instance().get_input_handler()->get_xpos() /
                            (double) window::get_instance().get_input_handler()->get_size_x()) * 2.f - 1.f;
            double new_y = 1.f - (window::get_instance().get_input_handler()->get_ypos() /
                                  (double) window::get_instance().get_input_handler()->get_size_y()) * 2.f;
            double old_x = new_x - x_offset;
            double old_y = new_y + y_offset;
            glm::mat4 inv_projection = glm::inverse(camera->get_proj_matrix());
            glm::mat4 inv_view = glm::inverse(camera->get_view_matrix());
            glm::mat4 inv_proj_view = inv_view * inv_projection;
            // plane to project on depends on the distance between the widget and camera
            float dist = glm::length(object->position - camera->get_camera_position());
            float scale = (dist / camera->far_clipping_dist - camera->near_clipping_dist) * 2.f - 1.f;
            glm::vec4 new_far = inv_proj_view * glm::vec4(new_x, new_y, scale, 1.f);
            glm::vec4 old_far = inv_proj_view * glm::vec4(old_x, old_y, scale, 1.f);
            // a and b are the old and new mouse positions projected on the far plane
            glm::vec3 a = glm::vec3(new_far.x / new_far.w, new_far.y / new_far.w, new_far.z / new_far.w);
            glm::vec3 b = glm::vec3(old_far.x / old_far.w, old_far.y / old_far.w, old_far.z / old_far.w);
            // scale the movement with the dot product of projected mouse movement and movement axis
            // do not normalize the direction as to scale with how fast the mouse is moving
            object->position += axis * (SENSITIVITY * glm::dot(a - b, axis));
        }
    }

    if (window::get_instance().get_input_handler()->get_mouse_button_state(input::LMB, input::RELEASED)) {
        dragging_widget = false;
    }
}
