#include "input.hpp"

input::input(GLFWwindow *p_window, uint32_t p_size_x, uint32_t p_size_y
) :
        window_handle(p_window), framebuffer_size_x(p_size_x), framebuffer_size_y(p_size_y)
{
    // initialize the bit vectors with the correct amount of memory
    pressed = (uint32_t *) calloc(VECTOR_COUNT, sizeof(uint32_t));
    down = (uint32_t *) calloc(VECTOR_COUNT, sizeof(uint32_t));
    released = (uint32_t *) calloc(VECTOR_COUNT, sizeof(uint32_t));
}

input::~input()
{
    free(pressed);
    free(down);
    free(released);
}

void input::pull_input()
{
    /** reset the inputs */
    // scroll offset
    scroll_x_offset = 0.;
    scroll_y_offset = 0.;

    // cursor position
    mouse_x_offset = 0.;
    mouse_y_offset = 0.;

    // keyboard and mouse buttons
    // pressed/released events are reset, down events are maintained
    memset(pressed, 0, VECTOR_COUNT * sizeof(uint32_t));
    memset(released, 0, VECTOR_COUNT * sizeof(uint32_t));

    // framebuffer
    framebuffer_resized = false;

    /** populate with new inputs */
    glfwPollEvents();
}

/**
 * Scroll offset.
 */

double input::get_xoffset() const
{
    return scroll_x_offset;
}

double input::get_yoffset() const
{
    return scroll_y_offset;
}

void input::set_scroll_offset(double p_xoffset, double p_yoffset)
{
    scroll_x_offset += p_xoffset;
    scroll_y_offset += p_yoffset;
}

/**
 * Cursor position.
 */

double input::get_xpos() const
{
    return mouse_x_pos;
}

double input::get_ypos() const
{
    return mouse_y_pos;
}

double input::get_offset_xpos() const
{
    return mouse_x_offset;
}

double input::get_offset_ypos() const
{
    return mouse_y_offset;
}

void input::set_cursor_position(double p_xpos, double p_ypos)
{
    mouse_x_pos = p_xpos;
    mouse_y_pos = p_ypos;

    mouse_x_offset = p_xpos - mouse_x_pos_last;
    mouse_y_offset = p_ypos - mouse_y_pos_last;

    mouse_x_pos_last = p_xpos;
    mouse_y_pos_last = p_ypos;
}

/**
 * Keyboard and mouse buttons.
 */

uint32_t *input::get_vector_from_state(key_state_t p_state)
{
    switch (p_state) {
        case PRESSED:
            return pressed;
        case RELEASED:
            return released;
        case DOWN:
            return down;
        default:
            nm_log::log(LOG_ERROR, "unknown key state %d\n", p_state);
            return NULL;
    }
}

void input::set_bit_state(uint32_t p_bit, key_state_t p_state, bool p_set)
{
    uint32_t *vector = get_vector_from_state(p_state);
    if (p_set) {
        vector[p_bit / 32] |= 1u << (p_bit % 32);
    } else {
        vector[p_bit / 32] &= ~(1u << (p_bit % 32));
    }
}

bool input::get_bit_state(uint32_t p_bit, key_state_t p_state)
{
    uint32_t *vector = get_vector_from_state(p_state);
    return vector[p_bit / 32] & (1u << (p_bit % 32));
}

void input::set_key_state(int p_value, key_state_t p_state, bool p_set)
{
    // {p_value} may take {GLFW_KEY_UNKNOWN} which is -1, which cannot be indexed
    if (p_value >= KEY_VALUES || p_value < 0) {
        nm_log::log(LOG_WARN, "cannot set key state for unknown value %d\n", p_value);
        return;
    }

    // safely promote signed integer to unsigned integer
    set_bit_state((uint32_t) p_value, p_state, p_set);
}

bool input::get_key_state(key_value_t p_value, key_state_t p_state)
{
    auto val = (int) p_value; // cast enum to integer
    // {p_value} may take {GLFW_KEY_UNKNOWN} which is -1, which cannot be indexed
    if (val >= KEY_VALUES || val < 0) {
        nm_log::log(LOG_WARN, "cannot get key state for unknown value %d\n", val);
        return false;
    }

    // safely promote signed integer to unsigned integer
    return get_bit_state((uint32_t) p_value, p_state);
}

void input::set_mouse_button_state(int p_value, key_state_t p_state, bool p_set)
{
    if (p_value >= MBN_VALUES) {
        nm_log::log(LOG_WARN, "cannot set mouse button state for unknown value %d\n", p_value);
        return;
    }

    // safely promote signed integer to unsigned integer (since mouse button value cannot be < 0 )
    set_bit_state((uint32_t) p_value + KEY_VALUES, p_state, p_set);
}

bool input::get_mouse_button_state(mouse_button_value_t p_value, key_state_t p_state)
{
    auto val = (int) p_value; // cast enum to integer
    if (val >= MBN_VALUES) {
        nm_log::log(LOG_WARN, "cannot get  mouse button state for unknown value %d\n", val);
        return false;
    }

    // safely promote signed integer to unsigned integer (since mouse button value cannot be < 0 )
    return get_bit_state((uint32_t) p_value + KEY_VALUES, p_state);
}

/**
 * Framebuffer.
 */

uint32_t input::get_size_x() const
{
    return framebuffer_size_x;
}

void input::set_size(uint32_t p_size_x, uint32_t p_size_y)
{
    input::framebuffer_size_x = p_size_x;
    input::framebuffer_size_y = p_size_y;
}

uint32_t input::get_size_y() const
{
    return framebuffer_size_y;
}

bool input::is_resized() const
{
    return framebuffer_resized;
}

void input::set_resized(bool p_resized)
{
    input::framebuffer_resized = p_resized;
}

bool input::is_iconified() const
{
    return framebuffer_iconified;
}

void input::set_iconified(bool p_iconified)
{
    input::framebuffer_iconified = p_iconified;
}
