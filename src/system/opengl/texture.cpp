#include "texture.hpp"

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

#include "../../util/nm_log.hpp"
#include "../../util/util.hpp"

int Texture::create_tex_from_file(
        Texture *tex, const char *tex_file,
        uint32_t channel_count, uint32_t bit_depth, uint32_t texture_unit)
{
    char *buffer = nullptr;
    size_t size;
    Util::read_file(&buffer, &size, tex_file);

    int rval = create_tex_from_mem(tex, buffer, size, channel_count, bit_depth, texture_unit);

    delete buffer;

    return rval;
}

int Texture::create_tex_from_mem(
        Texture *tex, const char *tex_data, size_t tex_len,
        uint32_t channel_count, uint32_t bit_depth, uint32_t texture_unit
)
{
    glGenTextures(1, &tex->m_tex_id);
    glBindTexture(GL_TEXTURE_2D, tex->m_tex_id);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // todo make this an option at some point
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // load and generate the texture
    int width, height, channel_count_;
    // NB: first pixel is top-left corner, flip it with this call
    stbi_set_flip_vertically_on_load(1);
    void *data;
    if (bit_depth == 8) {
        // unsigned char *
        default_depth:
        data = stbi_load_from_memory(
                (const unsigned char *) tex_data, tex_len, &width, &height, &channel_count_, channel_count);
    } else if (bit_depth == 16) {
        // unsigned short *
        data = stbi_load_16_from_memory(
                (const unsigned char *) tex_data, tex_len, &width, &height, &channel_count_, channel_count);
    } else {
        nm_log::log(LOG_WARN, "no stbi function found for specified bit depth, guessing 8 bit depth\n");
        goto default_depth;
    }


    if (channel_count_ != (int) channel_count) {
        nm_log::log(LOG_WARN,
                    "specified channel count (%d) not equal to found channel count (%d)\n",
                    channel_count, channel_count_, tex->m_tex_id);
    }

    // keep on moving forward with assumed channel count since this is the size of the buffer
    // since stbi may complain but will always allocate the number of channels the user specifies
    if (data) {
        GLenum format;
        GLenum internal_format;
        if (channel_count == 2) {
            format = GL_RG;
            internal_format = GL_RG;
        } else if (channel_count == 3) {
            format = GL_RGB;
            internal_format = GL_RGB;
        } else if (channel_count == 4) {
            format = GL_RGBA;
            internal_format = GL_RGBA;
        } else {
            nm_log::log(LOG_WARN, "unknown texture format, guessing GL_RGBA\n");
            format = GL_RGBA;
            internal_format = GL_RGBA;
        }

        GLenum type;
        if (bit_depth == 8) {
            type = GL_UNSIGNED_BYTE;
        } else if (bit_depth == 16) {
            type = GL_UNSIGNED_SHORT;
        } else {
            nm_log::log(LOG_WARN, "unknown texture type, guessing GL_UNSIGNED_BYTE\n");
            type = GL_UNSIGNED_BYTE;
        }

        // NB: first pixel is lower-left corner
        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, type, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        nm_log::log(LOG_ERROR, "failed to load texture\n");
        stbi_image_free(data);

        return EXIT_FAILURE;
    }

    stbi_image_free(data);

    tex->m_texture_unit = GL_TEXTURE0 + texture_unit;

    glBindTexture(GL_TEXTURE_2D, 0);

    return EXIT_SUCCESS;
}

void Texture::bind_tex(Texture *tex)
{
    glActiveTexture(tex->m_texture_unit);
    glBindTexture(GL_TEXTURE_2D, tex->m_tex_id);
}

void Texture::unbind_tex()
{
    // todo make active texture unit specific
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::delete_tex(Texture *tex)
{
    glDeleteTextures(1, &tex->m_tex_id);
}