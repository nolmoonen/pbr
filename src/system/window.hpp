#ifndef SYSTEM_WINDOW_HPP
#define SYSTEM_WINDOW_HPP

#include <cstdlib>

#include <glad/glad.h>

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>

#include "../util/nm_log.hpp"
#include "input.hpp"

/** Singleton class because of the need for static function pointers for GLFW callbacks. */
class window {
public:
    static window &get_instance();

    // delete copy constructor and assignment operator
    window(window const &) = delete;

    void operator=(window const &) = delete;

private:
    window() = default;

    GLFWwindow *window_handle = nullptr;

    /** True when window has been successfully initialized with {initialize}. */
    bool initialized = false;

    static constexpr const uint32_t INITIAL_WINDOW_SIZE_X = 800;
    static constexpr const uint32_t INITIAL_WINDOW_SIZE_Y = 600;

    input *input_handler = nullptr;

public:
    /** Call to {cleanup} is required if EXIT_SUCCESS is returned. */
    int32_t initialize();

    void cleanup();

    bool should_close();

    void set_to_close();

    void swap_buffers();

    void set_title(const char *p_title);

    input *get_input_handler() const;

private:

    /** Begin GLFW callbacks. */

    static void error_callback(int p_error, const char *p_description);

    static void key_callback(GLFWwindow *p_window, int p_key, int p_scancode, int p_action, int p_mods);

    static void scroll_callback(GLFWwindow *p_window, double p_xoffset, double p_yoffset);

    static void mouse_button_callback(GLFWwindow *p_window, int p_button, int p_action, int p_mods);

    static void cursor_position_callback(GLFWwindow *p_window, double p_xpos, double p_ypos);

    static void framebuffer_size_callback(GLFWwindow *p_window, int p_width, int p_height);

    static void window_iconify_callback(GLFWwindow *p_window, int p_iconified);

    /** End   GLFW callbacks. */
};


#endif //SYSTEM_WINDOW_HPP
