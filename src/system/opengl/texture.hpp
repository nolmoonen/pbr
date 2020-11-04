#ifndef SYSTEM_TEXTURE_HPP
#define SYSTEM_TEXTURE_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../manager/texture_manager.hpp"

class Texture {
public:
    /** Id of the texture. */
    GLuint tex_id;
    /** Texture unit. */
    GLenum texture_unit;
    /** Type: GL_TEXTURE_2D or GL_TEXTURE_CUBE_MAP */
    GLenum texture_type;

    /** Reads a file from disk into memory, calls {create_tex_from_mem} on that memory and deallocates the memory. */
    static int create_tex_from_file(
            Texture *tex, const char *tex_file,
            uint32_t channel_count, uint32_t bit_depth, uint32_t texture_unit, TextureManager::ChannelType channel_type,
            TextureManager::WrapType wrap_type);

    /** Uses stbi_image.h to read in texture data from memory and create a {GL_TEXTURE_2D} from it. */
    static int create_tex_from_mem(
            Texture *tex, const char *tex_data, size_t tex_len,
            uint32_t channel_count, uint32_t bit_depth, uint32_t texture_unit, TextureManager::ChannelType channel_type,
            TextureManager::WrapType wrap_type);

    /** Creates a {GL_TEXTURE2D} which is a 2D LUT for the BRDF equations used. */
    static int create_tex_from_tex(Texture *tex, Texture *resource_tex, uint32_t texture_unit);

    /** Projection and view matrices for capturing data onto the 6 cubemap face directions. */
    static const glm::mat4 CAPTURE_PROJECTION;
    static const glm::mat4 CAPTURE_VIEWS[];

    /** Creates a {GL_TEXTURE_CUBE_MAP} from {resource_tex}. */
    static int create_cubemap_from_tex(Texture *tex, Texture *resource_tex, uint32_t texture_unit);

    /** Creates a {GL_TEXTURE_CUBE_MAP} which is the irradiance calculated from {resource_tex}. */
    static int create_irradiance_cubemap_from_cubemap(Texture *tex, Texture *resource_tex, uint32_t texture_unit);

    /** Creates a {GL_TEXTURE_CUBE_MAP} which is the pre-filter calculated from {resource_tex}. */
    static int create_pre_filtered_cubemap_from_cubemap(Texture *tex, Texture *resource_tex, uint32_t texture_unit);

    static void bind_tex(Texture *tex);

    /** Unbinds the current texture. */
    static void unbind_tex(Texture *tex);

    static void delete_tex(Texture *tex);
};

#endif //SYSTEM_TEXTURE_HPP
