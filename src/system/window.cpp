#include "window.hpp"

window &window::get_instance()
{
    static window instance;
    return instance;
}

int32_t window::initialize()
{
    glfwSetErrorCallback(error_callback);

    if (glfwInit() == GLFW_FALSE) {
        nm_log::log(LOG_ERROR, "failed to initialize GLFW\n");
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    if ((window_handle = glfwCreateWindow(INITIAL_WINDOW_SIZE_X, INITIAL_WINDOW_SIZE_Y, "", NULL, NULL)) == NULL) {
        nm_log::log(LOG_ERROR, "failed to create window or OpenGl context\n");
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // create the input handler before registering the callbacks
    input_handler = new input(window_handle, INITIAL_WINDOW_SIZE_X, INITIAL_WINDOW_SIZE_Y);
    glfwSetKeyCallback(window_handle, key_callback);
    glfwSetScrollCallback(window_handle, scroll_callback);
    glfwSetMouseButtonCallback(window_handle, mouse_button_callback);
    glfwSetCursorPosCallback(window_handle, cursor_position_callback);
    glfwSetFramebufferSizeCallback(window_handle, framebuffer_size_callback);
    glfwSetWindowIconifyCallback(window_handle, window_iconify_callback);

    glfwMakeContextCurrent(window_handle);

    if (gladLoadGL() == 0) {
        nm_log::log(LOG_ERROR, "failed to load OpenGL extensions\n");
        delete input_handler;
        glfwDestroyWindow(window_handle);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwSwapInterval(1);

    // gl options
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glClearColor(0.0f, 0.1f, 0.2f, 1.0f);
    glViewport(0, 0, INITIAL_WINDOW_SIZE_X, INITIAL_WINDOW_SIZE_Y);

    initialized = true;

    return EXIT_SUCCESS;
}

void window::cleanup()
{
    if (!initialized) return;

    glfwDestroyWindow(window_handle);
    window_handle = nullptr;

    glfwTerminate();

    delete input_handler;
    input_handler = nullptr;

    initialized = false;
}

bool window::should_close()
{
    if (!initialized) return false;

    return glfwWindowShouldClose(window_handle) == GLFW_TRUE;
}

void window::set_to_close()
{
    if (!initialized) return;

    glfwSetWindowShouldClose(window_handle, GLFW_TRUE);
}

void window::swap_buffers()
{
    if (!initialized) return;

    glfwSwapBuffers(window_handle);
}

void window::set_title(const char *p_title)
{
    if (!initialized) return;

    glfwSetWindowTitle(window_handle, p_title);
}

input *window::get_input_handler() const
{
    return input_handler;
}

void window::error_callback(int p_error, const char *p_description)
{
    nm_log::log(LOG_ERROR, "glfw: %s\n", p_description);
}

void window::key_callback(GLFWwindow *p_window, int p_key, int p_scancode, int p_action, int p_mods)
{
    // per documentation: "The action is one of GLFW_PRESS, GLFW_REPEAT or GLFW_RELEASE."
    if (p_action == GLFW_PRESS) {
        window::get_instance().input_handler->set_key_state(p_key, input::PRESSED, true);
        window::get_instance().input_handler->set_key_state(p_key, input::DOWN, true);
    } else if (p_action == GLFW_RELEASE) {
        window::get_instance().input_handler->set_key_state(p_key, input::RELEASED, true);
        window::get_instance().input_handler->set_key_state(p_key, input::DOWN, false);
    }
}

void window::scroll_callback(GLFWwindow *p_window, double p_xoffset, double p_yoffset)
{
    window::get_instance().input_handler->set_scroll_offset(p_xoffset, p_yoffset);
}

void window::mouse_button_callback(GLFWwindow *p_window, int p_button, int p_action, int p_mods)
{
    // per documentation: "The action is one of GLFW_PRESS or GLFW_RELEASE."
    if (p_action == GLFW_PRESS) {
        window::get_instance().input_handler->set_mouse_button_state(p_button, input::PRESSED, true);
        window::get_instance().input_handler->set_mouse_button_state(p_button, input::DOWN, true);
    } else if (p_action == GLFW_RELEASE) {
        window::get_instance().input_handler->set_mouse_button_state(p_button, input::RELEASED, true);
        window::get_instance().input_handler->set_mouse_button_state(p_button, input::DOWN, false);
    }
}

void window::cursor_position_callback(GLFWwindow *p_window, double p_xpos, double p_ypos)
{
    window::get_instance().input_handler->set_cursor_position(p_xpos, p_ypos);
}

void window::framebuffer_size_callback(GLFWwindow *p_window, int p_width, int p_height)
{
    window::get_instance().input_handler->set_size(p_width, p_height);
    window::get_instance().input_handler->set_resized(true);
}

void window::window_iconify_callback(GLFWwindow *p_window, int p_iconified)
{
    window::get_instance().input_handler->set_iconified((bool) p_iconified);
}