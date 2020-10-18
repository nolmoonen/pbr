#include "texture.hpp"

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../../util/nm_log.hpp"
#include "../../util/util.hpp"
#include "shader.hpp"
#include "../manager/embedded.hpp"
#include "primitive/primitive.hpp"
#include "primitive/full_primitive.hpp"

int Texture::create_tex_from_file(
        Texture *tex, const char *tex_file,
        uint32_t channel_count, uint32_t bit_depth, uint32_t texture_unit, TextureManager::ChannelType channel_type,
        TextureManager::WrapType wrap_type)
{
    char *buffer = nullptr;
    size_t size;
    Util::read_file(&buffer, &size, tex_file);

    int rval = create_tex_from_mem(tex, buffer, size, channel_count, bit_depth, texture_unit, channel_type, wrap_type);

    delete buffer;

    return rval;
}

int Texture::create_tex_from_mem(
        Texture *tex, const char *tex_data, size_t tex_len,
        uint32_t channel_count, uint32_t bit_depth, uint32_t texture_unit, TextureManager::ChannelType channel_type,
        TextureManager::WrapType wrap_type
)
{
    tex->texture_type = GL_TEXTURE_2D;

    glGenTextures(1, &tex->tex_id);
    glBindTexture(tex->texture_type, tex->tex_id);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    if (wrap_type == TextureManager::REPEAT) {
        glTexParameteri(tex->texture_type, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(tex->texture_type, GL_TEXTURE_WRAP_T, GL_REPEAT);
    } else if (wrap_type == TextureManager::CLAMP) {
        glTexParameteri(tex->texture_type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(tex->texture_type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    } else {
        nm_log::log(LOG_WARN, "unrecognized wrapping type, guessing GL_REPEAT\n");
        glTexParameteri(tex->texture_type, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(tex->texture_type, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    glTexParameteri(tex->texture_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(tex->texture_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // load and generate the texture
    int found_width, found_height, found_channel_count;
    // NB: first pixel is top-left corner, flip it with this call
    stbi_set_flip_vertically_on_load(1);
    void *data;
    // todo could combine channel type and bit depth into a single enum variable
    if (bit_depth == 8 && channel_type == TextureManager::INTEGER) {
        // unsigned char *
        default_depth:
        data = stbi_load_from_memory(
                (const unsigned char *) tex_data, tex_len, &found_width, &found_height, &found_channel_count,
                channel_count);
    } else if (bit_depth == 16 && channel_type == TextureManager::INTEGER) {
        // unsigned short *
        data = stbi_load_16_from_memory(
                (const unsigned char *) tex_data, tex_len, &found_width, &found_height, &found_channel_count,
                channel_count);
    } else if (bit_depth == 32 && channel_type == TextureManager::FLOATING_POINT) {
        // float *
        data = stbi_loadf_from_memory(
                (const unsigned char *) tex_data, tex_len, &found_width, &found_height, &found_channel_count,
                channel_count);
    } else {
        nm_log::log(LOG_WARN, "no stbi function found for specified bit depth, guessing 8 bit depth\n");
        goto default_depth;
    }


    if (found_channel_count != (int) channel_count) {
        nm_log::log(LOG_WARN,
                    "specified channel count (%d) not equal to found channel count (%d)\n",
                    channel_count, found_channel_count, tex->tex_id);
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
        if (bit_depth == 8 && channel_type == TextureManager::INTEGER) {
            type = GL_UNSIGNED_BYTE;
        } else if (bit_depth == 16 && channel_type == TextureManager::INTEGER) {
            type = GL_UNSIGNED_SHORT;
        } else if (bit_depth == 32 && channel_type == TextureManager::FLOATING_POINT) {
            type = GL_FLOAT;
        } else {
            nm_log::log(LOG_WARN, "unknown texture type, guessing GL_UNSIGNED_BYTE\n");
            type = GL_UNSIGNED_BYTE;
        }

        // NB: first pixel is lower-left corner
        glTexImage2D(tex->texture_type, 0, internal_format, found_width, found_height, 0, format, type, data);
        glGenerateMipmap(tex->texture_type);
    } else {
        nm_log::log(LOG_ERROR, "failed to load texture\n");
        stbi_image_free(data);

        return EXIT_FAILURE;
    }

    stbi_image_free(data);

    tex->texture_unit = GL_TEXTURE0 + texture_unit;
    glBindTexture(tex->texture_type, 0);

    return EXIT_SUCCESS;
}

int Texture::create_cubemap_from_tex(Texture *tex, Texture *resource_tex, uint32_t texture_unit)
{
    const uint32_t RES_X = 512;
    const uint32_t RES_Y = 512;
    tex->texture_type = GL_TEXTURE_CUBE_MAP;

    /** setup framebuffer */
    GLuint captureFBO;
    GLuint captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, RES_X, RES_Y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    /** setup cubemap to render to and attach to framebuffer */
    glGenTextures(1, &tex->tex_id);
    glBindTexture(tex->texture_type, tex->tex_id);
    for (uint32_t i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, RES_X, RES_Y, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(tex->texture_type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(tex->texture_type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(tex->texture_type, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(tex->texture_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(tex->texture_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /** set up projection and view matrices for capturing data onto the 6 cubemap face directions */
    glm::mat4 capture_projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 capture_views[] = {
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
    };

    /** convert HDR equirectangular environment map to cubemap equivalent */
    // hacky way to manually create shader since we do not have access to a shader manager instance
    ShaderProgram shader{};
    ShaderProgram::create_shader_program(
            &shader, equirectangular_map_vert, equirectangular_map_vert_len,
            equirectangular_map_frag, equirectangular_map_frag_len);
    ShaderProgram::use_shader_program(&shader);
    ShaderProgram::set_int(&shader, "equirectangular_map", 0);
    ShaderProgram::set_mat4(&shader, "projection_matrix", capture_projection);
    // apply a scaling with .5 since code expects a 1x1x1 cube (our unit cube is 2x2x2)
    ShaderProgram::set_mat4(&shader, "model_matrix", glm::scale(glm::identity<glm::mat4>(), glm::vec3(.5f)));

    // find the previously obtained texture resource
    Texture::bind_tex(resource_tex);

    // hacky way to manually create mesh since we do not have access to a primitive manager instance
    Primitive *skybox = FullPrimitive::create_skybox();

    // save the current viewport dims to later restore it
    GLfloat viewport_dims[4];
    glGetFloatv(GL_VIEWPORT, viewport_dims);

    glViewport(0, 0, RES_X, RES_Y); // configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (uint32_t i = 0; i < 6; i++) {
        ShaderProgram::set_mat4(&shader, "view_matrix", capture_views[i]);
        glFramebufferTexture2D(
                GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, tex->tex_id, 0);
        glClear((unsigned) GL_COLOR_BUFFER_BIT | (unsigned) GL_DEPTH_BUFFER_BIT);

        skybox->render_primitive();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // manually delete primitive since it has not been registered in a primitive manager instance
    skybox->delete_primitive();

    // manually delete shader instance since it has not been registered in a shader manager instance
    ShaderProgram::delete_shader_program(&shader);

    // restore viewport
    glViewport(viewport_dims[0], viewport_dims[1], viewport_dims[2], viewport_dims[3]);

    tex->texture_unit = GL_TEXTURE0 + texture_unit;

    return EXIT_SUCCESS;
}

int Texture::create_irradiance_cubemap_from_cubemap(Texture *tex, Texture *resource_tex, uint32_t texture_unit)
{
    const uint32_t RES_X = 32;
    const uint32_t RES_Y = 32;
    tex->texture_type = GL_TEXTURE_CUBE_MAP;

    /** setup framebuffer */
    GLuint captureFBO;
    GLuint captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, RES_X, RES_Y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    /** setup cubemap to render to and attach to framebuffer */
    glGenTextures(1, &tex->tex_id);
    glBindTexture(tex->texture_type, tex->tex_id);
    for (uint32_t i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, RES_X, RES_Y, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(tex->texture_type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(tex->texture_type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(tex->texture_type, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(tex->texture_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(tex->texture_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /** set up projection and view matrices for capturing data onto the 6 cubemap face directions */
    glm::mat4 capture_projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 capture_views[] = {
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
    };

    /** convert HDR cubemap to irradiance cubemap equivalent */
    // hacky way to manually create shader since we do not have access to a shader manager instance
    ShaderProgram shader{};
    ShaderProgram::create_shader_program(
            &shader, irradiance_map_vert, irradiance_map_vert_len,
            irradiance_map_frag, irradiance_map_frag_len);
    ShaderProgram::use_shader_program(&shader);
    ShaderProgram::set_int(&shader, "environment_map", 0);
    ShaderProgram::set_mat4(&shader, "projection_matrix", capture_projection);
    // apply a scaling with .5 since code expects a 1x1x1 cube (our unit cube is 2x2x2)
    ShaderProgram::set_mat4(&shader, "model_matrix", glm::scale(glm::identity<glm::mat4>(), glm::vec3(.5f)));

    // find the previously obtained texture resource
    Texture::bind_tex(resource_tex);

    // hacky way to manually create mesh since we do not have access to a primitive manager instance
    Primitive *skybox = FullPrimitive::create_skybox();

    // save the current viewport dims to later restore it
    GLfloat viewport_dims[4];
    glGetFloatv(GL_VIEWPORT, viewport_dims);

    glViewport(0, 0, RES_X, RES_Y); // configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (uint32_t i = 0; i < 6; i++) {
        ShaderProgram::set_mat4(&shader, "view_matrix", capture_views[i]);
        glFramebufferTexture2D(
                GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, tex->tex_id, 0);
        glClear((unsigned) GL_COLOR_BUFFER_BIT | (unsigned) GL_DEPTH_BUFFER_BIT);

        skybox->render_primitive();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // manually delete primitive since it has not been registered in a primitive manager instance
    skybox->delete_primitive();

    // manually delete shader instance since it has not been registered in a shader manager instance
    ShaderProgram::delete_shader_program(&shader);

    // restore viewport
    glViewport(viewport_dims[0], viewport_dims[1], viewport_dims[2], viewport_dims[3]);

    tex->texture_unit = GL_TEXTURE0 + texture_unit;

    return EXIT_SUCCESS;
}

void Texture::bind_tex(Texture *tex)
{
    glActiveTexture(tex->texture_unit);
    glBindTexture(tex->texture_type, tex->tex_id);
}

void Texture::unbind_tex(Texture *tex)
{
    glActiveTexture(tex->texture_unit);
    glBindTexture(tex->texture_type, 0);
}

void Texture::delete_tex(Texture *tex)
{
    glDeleteTextures(1, &tex->tex_id);
}