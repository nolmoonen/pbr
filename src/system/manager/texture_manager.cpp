#include "texture_manager.hpp"

#include "embedded.hpp"
#include "../opengl/texture.hpp"

// provide a default destructor for the base class
template<>
inline Manager<Texture>::~Manager<Texture>() = default;

TextureManager::TextureResource::TextureResource(
        uint32_t channels, uint32_t bit_depth, uint32_t texture_unit, ChannelType type, WrapType wrap_type
) : channels(channels), bit_depth(bit_depth), texture_unit(texture_unit), type(type), wrap_type(wrap_type)
{}

TextureManager::TextureResourceFromMemory::TextureResourceFromMemory(
        uint32_t channels, uint32_t bit_depth, uint32_t texture_unit, ChannelType type, WrapType wrap_type,
        const char *text, const size_t *len
) : TextureResource(channels, bit_depth, texture_unit, type, wrap_type), text(text), len(len)
{}

int TextureManager::TextureResourceFromMemory::create_texture(Texture *texture) const
{
    return Texture::create_tex_from_mem(texture, text, *len, channels, bit_depth, texture_unit, type, wrap_type);
}

TextureManager::TextureResourceFromFile::TextureResourceFromFile(
        uint32_t channels, uint32_t bit_depth, uint32_t texture_unit, ChannelType type, WrapType wrap_type,
        const char *file_name
) : TextureResource(channels, bit_depth, texture_unit, type, wrap_type), file_name(file_name)
{}

int TextureManager::TextureResourceFromFile::create_texture(Texture *texture) const
{
    return Texture::create_tex_from_file(texture, file_name, channels, bit_depth, texture_unit, type, wrap_type);
}

TextureManager::TextureResourceFromTextureResource::TextureResourceFromTextureResource(
        TextureType texture_type, uint32_t texture_unit, int (*create_function)(Texture *, Texture *, uint32_t)
) :
        TextureResource(0, 0, texture_unit, static_cast<ChannelType>(0), static_cast<WrapType>(0)),
        texture_type(texture_type), create_function(create_function)
{}

int TextureManager::TextureResourceFromTextureResource::create_texture(Texture *texture) const
{
    // create resource texture without registering in the texture manager instance
    Texture resource_texture{};
    auto resource = TEXTURE_RESOURCES.find(texture_type);
    if (resource == TEXTURE_RESOURCES.end()) {
        nm_log::log(LOG_ERROR, "texture type cannot be found in defined texture resources\n");
    }
    resource->second->create_texture(&resource_texture);

    // create the desired texture using obtained texture
    int rval = create_function(texture, &resource_texture, texture_unit);

    // manually delete resource texture since it has not been registered in texture manager instance
    Texture::delete_tex(&resource_texture);

    return rval;
}

const std::map<uint32_t, TextureManager::TextureResource *> TextureManager::TEXTURE_RESOURCES = {
        {TEXTURE_TEST,
                new TextureResourceFromMemory(4, 8, 0, INTEGER, REPEAT, test_png, &test_png_len)},

        {TEXTURE_CAYLEY_INTERIOR_HDR,
                new TextureResourceFromMemory(
                        3, 32, 0, FLOATING_POINT, CLAMP, cayley_interior_hdr, &cayley_interior_hdr_len)},
        {CUBEMAP_CAYLEY_INTERIOR,
                new TextureResourceFromTextureResource(
                        TEXTURE_CAYLEY_INTERIOR_HDR, 0, &Texture::create_cubemap_from_tex)},
        {CUBEMAP_CAYLEY_INTERIOR_IRRADIANCE,
                new TextureResourceFromTextureResource(
                        CUBEMAP_CAYLEY_INTERIOR, 5, &Texture::create_irradiance_cubemap_from_cubemap)},

        {TEXTURE_STUDIO_HDR,
                new TextureResourceFromMemory(
                        3, 32, 0, FLOATING_POINT, CLAMP, studio_hdr, &studio_hdr_len)},
        {CUBEMAP_STUDIO,
                new TextureResourceFromTextureResource(
                        TEXTURE_STUDIO_HDR, 0, &Texture::create_cubemap_from_tex)},
        {CUBEMAP_STUDIO_IRRADIANCE,
                new TextureResourceFromTextureResource(
                        CUBEMAP_STUDIO, 5, &Texture::create_irradiance_cubemap_from_cubemap)},

        {TEXTURE_BRICK_1_DIFF,
                new TextureResourceFromMemory(4, 16, 0, INTEGER, REPEAT, brick_diff_png, &brick_diff_png_len)},
        {TEXTURE_BRICK_1_NORM,
                new TextureResourceFromMemory(4, 16, 1, INTEGER, REPEAT, brick_norm_png, &brick_norm_png_len)},
        {TEXTURE_BRICK_1_AO,
                new TextureResourceFromMemory(2, 16, 2, INTEGER, REPEAT, brick_ao_png, &brick_ao_png_len)},
        {TEXTURE_BRICK_1_ROUGH,
                new TextureResourceFromMemory(2, 16, 3, INTEGER, REPEAT, brick_rough_png, &brick_rough_png_len)},
        {TEXTURE_BRICK_1_DISP,
                new TextureResourceFromMemory(2, 16, 4, INTEGER, REPEAT, brick_disp_png, &brick_disp_png_len)},

        {TEXTURE_BRICK_2_DIFF,
                new TextureResourceFromFile(4, 16, 0, INTEGER, REPEAT,
                                            "../res/tex/castle_brick_07/castle_brick_07_2k_png/castle_brick_07_diff_2k.png")},
        {TEXTURE_BRICK_2_NORM,
                new TextureResourceFromFile(4, 16, 1, INTEGER, REPEAT,
                                            "../res/tex/castle_brick_07/castle_brick_07_2k_png/castle_brick_07_nor_2k.png")},
        {TEXTURE_BRICK_2_AO,
                new TextureResourceFromFile(4, 16, 2, INTEGER,
                                            REPEAT, // NB: intentional difference with {TEXTURE_BRICK_AO}
                                            "../res/tex/castle_brick_07/castle_brick_07_2k_png/castle_brick_07_ao_2k.png")},
        {TEXTURE_BRICK_2_ROUGH,
                new TextureResourceFromFile(2, 16, 3, INTEGER, REPEAT,
                                            "../res/tex/castle_brick_07/castle_brick_07_2k_png/castle_brick_07_rough_2k.png")},
        {TEXTURE_BRICK_2_DISP,
                new TextureResourceFromFile(2, 16, 4, INTEGER, REPEAT,
                                            "../res/tex/castle_brick_07/castle_brick_07_2k_png/castle_brick_07_disp_2k.png")},

        {TEXTURE_BRICK_4_DIFF,
                new TextureResourceFromFile(4, 16, 0, INTEGER, REPEAT,
                                            "../res/tex/castle_brick_07/castle_brick_07_4k_png/castle_brick_07_diff_4k.png")},
        {TEXTURE_BRICK_4_NORM,
                new TextureResourceFromFile(4, 16, 1, INTEGER, REPEAT,
                                            "../res/tex/castle_brick_07/castle_brick_07_4k_png/castle_brick_07_nor_4k.png")},
        {TEXTURE_BRICK_4_AO,
                new TextureResourceFromFile(4, 16, 2, INTEGER,
                                            REPEAT, // NB: intentional difference with {TEXTURE_BRICK_AO}
                                            "../res/tex/castle_brick_07/castle_brick_07_4k_png/castle_brick_07_ao_4k.png")},
        {TEXTURE_BRICK_4_ROUGH,
                new TextureResourceFromFile(2, 16, 3, INTEGER, REPEAT,
                                            "../res/tex/castle_brick_07/castle_brick_07_4k_png/castle_brick_07_rough_4k.png")},
        {TEXTURE_BRICK_4_DISP,
                new TextureResourceFromFile(2, 16, 4, INTEGER, REPEAT,
                                            "../res/tex/castle_brick_07/castle_brick_07_4k_png/castle_brick_07_disp_4k.png")},

        {TEXTURE_BRICK_8_DIFF,
                new TextureResourceFromFile(4, 16, 0, INTEGER, REPEAT,
                                            "../res/tex/castle_brick_07/castle_brick_07_8k_png/castle_brick_07_diff_8k.png")},
        {TEXTURE_BRICK_8_NORM,
                new TextureResourceFromFile(4, 16, 1, INTEGER, REPEAT,
                                            "../res/tex/castle_brick_07/castle_brick_07_8k_png/castle_brick_07_nor_8k.png")},
        {TEXTURE_BRICK_8_AO,
                new TextureResourceFromFile(4, 16, 2, INTEGER,
                                            REPEAT, // NB: intentional difference with {TEXTURE_BRICK_AO}
                                            "../res/tex/castle_brick_07/castle_brick_07_8k_png/castle_brick_07_ao_8k.png")},
        {TEXTURE_BRICK_8_ROUGH,
                new TextureResourceFromFile(2, 16, 3, INTEGER, REPEAT,
                                            "../res/tex/castle_brick_07/castle_brick_07_8k_png/castle_brick_07_rough_8k.png")},
        {TEXTURE_BRICK_8_DISP,
                new TextureResourceFromFile(2, 16, 4, INTEGER, REPEAT,
                                            "../res/tex/castle_brick_07/castle_brick_07_8k_png/castle_brick_07_disp_8k.png")},
};

int32_t TextureManager::create_item(Texture **item, uint32_t id)
{
    // find index of texture
    auto entry = TEXTURE_RESOURCES.find(id);
    if (entry == TEXTURE_RESOURCES.end()) {
        nm_log::log(LOG_ERROR, "\"%d\" is not a registered texture id\n", id);

        return EXIT_FAILURE;
    }

    // else, create from function pointer
    *item = new Texture();
    int32_t rval = entry->second->create_texture(*item);
    if (rval == EXIT_FAILURE) {
        nm_log::log(LOG_ERROR, "failed to create texture\n");

        delete *item;

        return EXIT_FAILURE;
    }

    nm_log::log(LOG_INFO, "created texture with id \"%d\"\n", id);

    return EXIT_SUCCESS;
}

void TextureManager::delete_item(Texture **item, uint32_t id)
{
    delete_item_self(item, id);
}

void TextureManager::delete_item_self(Texture **item, uint32_t id)
{
    Texture::delete_tex(*item); // delete the created data associated with this handle
    delete (*item);             // delete the handle itself

    nm_log::log(LOG_INFO, "deleted texture with id \"%d\"\n", id);
}

TextureManager::~TextureManager()
{
    for (auto &item : map) {
        delete_item_self(&item.second.item, item.first);
    }
}