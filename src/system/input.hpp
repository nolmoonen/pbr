#ifndef SYSTEM_INPUT_HPP
#define SYSTEM_INPUT_HPP

#include <cstdlib>
#include <cstdint>
#include <cstring>

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>

#include "../util/nm_log.hpp"

class input {
private:
    GLFWwindow *window_handle;
public:
    input(GLFWwindow *p_window, uint32_t p_size_x, uint32_t p_size_y);

    virtual ~input();

    /** Clear the input, populate it with new input. */
    void pull_input();

    /**
     * Scroll offset.
     */
private:
    // the offset produced by scrolling since the last pull
    double scroll_x_offset = 0.;
    double scroll_y_offset = 0.;
public:
    double get_xoffset() const;

    double get_yoffset() const;

    void set_scroll_offset(double p_xoffset, double p_yoffset);

    /**
     * Cursor position.
     */
private:
    // the position of the cursor at the next pull
    // todo is this initialization correct?
    double mouse_x_pos = 0.;
    double mouse_y_pos = 0.;
    // the position of the cursor at the last pull
    // todo is this initialization correct?
    double mouse_x_pos_last = 0.;
    double mouse_y_pos_last = 0.;
    // the offset produced by the cursor since the last pull
    double mouse_x_offset = 0.;
    double mouse_y_offset = 0.;
public:

    double get_xpos() const;

    double get_ypos() const;

    double get_offset_xpos() const;

    double get_offset_ypos() const;

    void set_cursor_position(double p_xpos, double p_ypos);

    /**
     * Keyboard and mouse buttons.
     * Grouped together since they state representation is the same.
     */
private:
    // number of key values defined in glfw
    static constexpr const uint32_t KEY_VALUES = GLFW_KEY_LAST + 1;
    // number of mouse button values defined in glfw
    static constexpr const uint32_t MBN_VALUES = GLFW_MOUSE_BUTTON_LAST + 1;
    // total number of keys tracked by the vectors
    static constexpr const uint32_t KEYS = KEY_VALUES + MBN_VALUES;
    // number of 32 bits vectors are needed to maintain all key values
    static constexpr uint32_t VECTOR_COUNT = KEYS / 32 + ((KEYS % 32) ? 1 : 0);

    /**
     * bit vectors storing the state of the keys and mouse buttons
     * first all of the keys, then all of the mouse buttons */
    uint32_t *pressed;  // whether key has been pressed at least once since last pull
    uint32_t *released; // whether key has been released at least once since last pull
    uint32_t *down;     // whether key is down at next pull
public:
    /**
     * This forms an interface of {input}. It maintains the state of all keyboard input, but the GLFW
     * side is hidden. To add new keys or mouse buttons, simply add the mapping to the corresponding
     * enum. */
    enum key_value_t {
        ESCAPE = GLFW_KEY_ESCAPE,
        D = GLFW_KEY_D,
        SPACE = GLFW_KEY_SPACE,
        BACKSPACE = GLFW_KEY_BACKSPACE,
        RIGHT = GLFW_KEY_RIGHT,
    };
    enum mouse_button_value_t {
        LMB = GLFW_MOUSE_BUTTON_LEFT,
        MMB = GLFW_MOUSE_BUTTON_MIDDLE,
        RMB = GLFW_MOUSE_BUTTON_RIGHT
    };

    /** The states the keys and mouse buttons can be in, and are recorded for. */
    enum key_state_t {
        PRESSED,
        RELEASED,
        DOWN
    };
private:
    uint32_t *get_vector_from_state(key_state_t p_state);

    /**
     * {p_bit} is index of a bit in the state vectors.
     * {p_state} determines which state vector is updated.
     * {p_set} whether to set or unset the bit.
     */
    void set_bit_state(uint32_t p_bit, key_state_t p_state, bool p_set);

    bool get_bit_state(uint32_t p_bit, key_state_t p_state);

public:
    /**
     * {p_value} is a GLFW key value (https://www.glfw.org/docs/3.3/group__keys.html).
     * {p_set} whether to enable the bit in the vector.
     * */
    void set_key_state(int p_value, key_state_t p_state, bool p_set);

    bool get_key_state(key_value_t p_value, key_state_t p_state);

    /** analogue to {set_key_state} (https://www.glfw.org/docs/3.3/group__buttons.html)*/
    void set_mouse_button_state(int p_value, key_state_t p_state, bool p_set);

    /** analogue to {get_key_state} */
    bool get_mouse_button_state(mouse_button_value_t p_value, key_state_t p_state);

    /**
     * Framebuffer.
     */
private:
    uint32_t framebuffer_size_x;
    uint32_t framebuffer_size_y;
    bool framebuffer_resized = false;   // whether resized since the last pull
    bool framebuffer_iconified = false; // whether window is iconified at the last pull
public:
    uint32_t get_size_x() const;

    uint32_t get_size_y() const;

    void set_size(uint32_t p_size_x, uint32_t p_size_y);

    bool is_resized() const;

    void set_resized(bool p_resized);

    bool is_iconified() const;

    void set_iconified(bool p_iconified);
};

#endif //SYSTEM_INPUT_HPP
