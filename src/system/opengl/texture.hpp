#ifndef SYSTEM_TEXTURE_HPP
#define SYSTEM_TEXTURE_HPP

#include <glad/glad.h>

struct Texture {
    /** Id of the texture. */
    GLuint m_tex_id;
    /** Texture unit. */
    GLenum m_texture_unit;

    /** cannot be used with embedded resources */
    static int create_tex_from_file(
            Texture *tex, const char *tex_file,
            uint32_t channel_count, uint32_t bit_depth, uint32_t texture_unit);

    static int create_tex_from_mem(
            Texture *tex, const char *tex_data, size_t tex_len,
            uint32_t channel_count, uint32_t bit_depth, uint32_t texture_unit);

    static void bind_tex(Texture *tex);

    /** Unbinds the current texture. */
    static void unbind_tex();

    static void delete_tex(Texture *tex);
};

#endif //SYSTEM_TEXTURE_HPP
