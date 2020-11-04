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

const glm::mat4 Texture::CAPTURE_PROJECTION = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
const glm::mat4 Texture::CAPTURE_VIEWS[] = {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
};

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

int Texture::create_tex_from_tex(Texture *tex, Texture *resource_tex, uint32_t texture_unit)
{
    const uint32_t RES_X = 512;
    const uint32_t RES_Y = 512;

    /** generate texture */
    glGenTextures(1, &tex->tex_id);
    tex->texture_type = GL_TEXTURE_2D;
    glBindTexture(tex->texture_type, tex->tex_id);

    /** setup framebuffer */
    GLuint captureFBO;
    GLuint captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, RES_X, RES_Y);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex->texture_type, tex->tex_id, 0);

    /** setup image to render to */
    glTexImage2D(tex->texture_type, 0, GL_RG16F, RES_X, RES_Y, 0, GL_RG, GL_FLOAT, 0);
    glTexParameteri(tex->texture_type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(tex->texture_type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(tex->texture_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(tex->texture_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // save the current viewport dims to later restore it
    GLfloat viewport_dims[4];
    glGetFloatv(GL_VIEWPORT, viewport_dims);

    glViewport(0, 0, RES_X, RES_Y);
    ShaderProgram shader{};
    ShaderProgram::create_shader_program(&shader, brdf_vert, brdf_vert_len, brdf_frag, brdf_frag_len);
    ShaderProgram::use_shader_program(&shader);

    glClear((unsigned) GL_COLOR_BUFFER_BIT | (unsigned) GL_DEPTH_BUFFER_BIT);

    // begin render_quad
    GLuint quad_vao = 0;
    GLuint quad_vbo;
    float quad_vertices[] = {
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };
    glGenVertexArrays(1, &quad_vao);
    glBindVertexArray(quad_vao);

    glGenBuffers(1, &quad_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));

    glBindVertexArray(quad_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    glDeleteVertexArrays(1, &quad_vao); // todo is this a full cleanup?
    // end   render_quad

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    ShaderProgram::delete_shader_program(&shader);

    // restore viewport
    glViewport(viewport_dims[0], viewport_dims[1], viewport_dims[2], viewport_dims[3]);

    tex->texture_unit = GL_TEXTURE0 + texture_unit;

    return EXIT_SUCCESS;
}

int Texture::create_cubemap_from_tex(Texture *tex, Texture *resource_tex, uint32_t texture_unit)
{
    const uint32_t RES_X = 512;
    const uint32_t RES_Y = 512;

    /** generate texture */
    glGenTextures(1, &tex->tex_id);
    tex->texture_type = GL_TEXTURE_CUBE_MAP;
    glBindTexture(tex->texture_type, tex->tex_id);

    /** setup framebuffer */
    GLuint captureFBO;
    GLuint captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, RES_X, RES_Y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    /** setup cubemap to render to */
    for (uint32_t i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, RES_X, RES_Y, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(tex->texture_type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(tex->texture_type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(tex->texture_type, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(tex->texture_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // see below glGenerateMipmap
    glTexParameteri(tex->texture_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /** convert HDR equirectangular environment map to cubemap equivalent */
    // hacky way to manually create shader since we do not have access to a shader manager instance
    ShaderProgram shader{};
    ShaderProgram::create_shader_program(
            &shader, equirectangular_map_vert, equirectangular_map_vert_len,
            equirectangular_map_frag, equirectangular_map_frag_len);
    ShaderProgram::use_shader_program(&shader);
    ShaderProgram::set_int(&shader, "equirectangular_map", 0);
    ShaderProgram::set_mat4(&shader, "projection_matrix", CAPTURE_PROJECTION);
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
        ShaderProgram::set_mat4(&shader, "view_matrix", CAPTURE_VIEWS[i]);
        glFramebufferTexture2D(
                GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, tex->tex_id, 0);
        glClear((unsigned) GL_COLOR_BUFFER_BIT | (unsigned) GL_DEPTH_BUFFER_BIT);

        skybox->render_primitive();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // enable mipmap to later sample different levels in pre-filter map
    glBindTexture(tex->texture_type, tex->tex_id);
    glGenerateMipmap(tex->texture_type);

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

    /** generate texture */
    glGenTextures(1, &tex->tex_id);
    tex->texture_type = GL_TEXTURE_CUBE_MAP;
    glBindTexture(tex->texture_type, tex->tex_id);

    /** setup framebuffer */
    GLuint captureFBO;
    GLuint captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, RES_X, RES_Y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    /** setup cubemap to render to */
    for (uint32_t i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, RES_X, RES_Y, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(tex->texture_type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(tex->texture_type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(tex->texture_type, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(tex->texture_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(tex->texture_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /** convert HDR cubemap to irradiance cubemap equivalent */
    // hacky way to manually create shader since we do not have access to a shader manager instance
    ShaderProgram shader{};
    ShaderProgram::create_shader_program(
            &shader, irradiance_map_vert, irradiance_map_vert_len,
            irradiance_map_frag, irradiance_map_frag_len);
    ShaderProgram::use_shader_program(&shader);
    ShaderProgram::set_int(&shader, "environment_map", 0);
    ShaderProgram::set_mat4(&shader, "projection_matrix", CAPTURE_PROJECTION);
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
        ShaderProgram::set_mat4(&shader, "view_matrix", CAPTURE_VIEWS[i]);
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

int Texture::create_pre_filtered_cubemap_from_cubemap(Texture *tex, Texture *resource_tex, uint32_t texture_unit)
{
    const uint32_t RES_X = 128;
    const uint32_t RES_Y = 128;

    glGenTextures(1, &tex->tex_id);
    tex->texture_type = GL_TEXTURE_CUBE_MAP;
    glBindTexture(tex->texture_type, tex->tex_id);

    /** setup framebuffer */
    GLuint captureFBO;
    GLuint captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    // {glRenderbufferStorage} is performed later due to changing framebuffer sizes
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    /** setup cubemap to render to and attach to framebuffer */
    for (uint32_t i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, RES_X, RES_Y, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(tex->texture_type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(tex->texture_type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(tex->texture_type, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // we can use trilinear filtering since we are using mipmaps
    glTexParameteri(tex->texture_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(tex->texture_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(tex->texture_type); // this cubemap is mipmapped, with increasing roughness values

    /** convert HDR cubemap to pre-filter cubemap equivalent */
    // hacky way to manually create shader since we do not have access to a shader manager instance
    ShaderProgram shader{};
    ShaderProgram::create_shader_program(
            &shader, pre_filter_map_vert, pre_filter_map_vert_len,
            pre_filter_map_frag, pre_filter_map_frag_len);
    ShaderProgram::use_shader_program(&shader);
    ShaderProgram::set_int(&shader, "environment_map", 0);
    ShaderProgram::set_mat4(&shader, "projection_matrix", CAPTURE_PROJECTION);
    // apply a scaling with .5 since code expects a 1x1x1 cube (our unit cube is 2x2x2)
    ShaderProgram::set_mat4(&shader, "model_matrix", glm::scale(glm::identity<glm::mat4>(), glm::vec3(.5f)));

    // find the previously obtained texture resource
    Texture::bind_tex(resource_tex);

    // hacky way to manually create mesh since we do not have access to a primitive manager instance
    Primitive *skybox = FullPrimitive::create_skybox();

    // save the current viewport dims to later restore it
    GLfloat viewport_dims[4];
    glGetFloatv(GL_VIEWPORT, viewport_dims);

    const uint32_t MAX_MIP_LEVELS = 5;
    uint32_t mip_width = RES_X;
    uint32_t mip_height = RES_Y;
    for (uint32_t mip = 0; mip < MAX_MIP_LEVELS; mip++) {
        // resize framebuffer according to mip-level size
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mip_width, mip_height);
        glViewport(0, 0, mip_width, mip_height);

        float roughness = (float) mip / (float) (MAX_MIP_LEVELS - 1);
        ShaderProgram::set_float(&shader, "roughness", roughness);
        for (uint32_t i = 0; i < 6; i++) {
            ShaderProgram::set_mat4(&shader, "view_matrix", CAPTURE_VIEWS[i]);
            glFramebufferTexture2D(
                    GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, tex->tex_id, mip);
            glClear((unsigned) GL_COLOR_BUFFER_BIT | (unsigned) GL_DEPTH_BUFFER_BIT);

            skybox->render_primitive();
        }

        mip_width /= 2;
        mip_height /= 2;
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