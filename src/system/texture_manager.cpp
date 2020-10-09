#include "texture_manager.hpp"

// provide a default destructor for the base class
template<>
inline Manager<Texture>::~Manager<Texture>() = default;

TextureManager::TextureResource::TextureResource(
        uint32_t id, uint32_t channels, uint32_t bit_depth, uint32_t texture_unit
) : id(id), channels(channels), bit_depth(bit_depth), texture_unit(texture_unit)
{}

TextureManager::TextureResourceFromMemory::TextureResourceFromMemory(
        uint32_t id, uint32_t channels, uint32_t bit_depth, uint32_t texture_unit, const char *text, const size_t *len
) : TextureResource(id, channels, bit_depth, texture_unit), text(text), len(len)
{}

int TextureManager::TextureResourceFromMemory::create_texture(Texture *texture) const
{
    return Texture::create_tex_from_mem(texture, text, *len, channels, bit_depth, texture_unit);
}

TextureManager::TextureResourceFromFile::TextureResourceFromFile(
        uint32_t id, uint32_t channels, uint32_t bit_depth, uint32_t texture_unit, const char *file_name
) : TextureResource(id, channels, bit_depth, texture_unit), file_name(file_name)
{}

int TextureManager::TextureResourceFromFile::create_texture(Texture *texture) const
{
    return Texture::create_tex_from_file(texture, file_name, channels, bit_depth, texture_unit);
}

const TextureManager::TextureResource *TextureManager::TEXTURE_RESOURCES[] = {
        new TextureResourceFromMemory(TEXTURE_TEST, 4, 8, 0, test_png, &test_png_len),

        new TextureResourceFromMemory(TEXTURE_BRICK_DIFF, 4, 16, 0, brick_diff_png, &brick_diff_png_len),
        new TextureResourceFromMemory(TEXTURE_BRICK_NORM, 4, 16, 1, brick_norm_png, &brick_norm_png_len),
        new TextureResourceFromMemory(TEXTURE_BRICK_AO, 2, 16, 2, brick_ao_png, &brick_ao_png_len),
        new TextureResourceFromMemory(TEXTURE_BRICK_ROUGH, 2, 16, 3, brick_rough_png, &brick_rough_png_len),

        new TextureResourceFromFile(TEXTURE_BRICK_8_DIFF, 4, 16, 0,
                                    "../res/tex/castle_brick_07_8k_png/castle_brick_07_diff_8k.png"),
        new TextureResourceFromFile(TEXTURE_BRICK_8_NORM, 4, 16, 1,
                                    "../res/tex/castle_brick_07_8k_png/castle_brick_07_nor_8k.png"),
        new TextureResourceFromFile(TEXTURE_BRICK_8_AO, 4, 16, 2, // NB: intentional difference with {TEXTURE_BRICK_AO}
                                    "../res/tex/castle_brick_07_8k_png/castle_brick_07_ao_8k.png"),
        new TextureResourceFromFile(TEXTURE_BRICK_8_ROUGH, 2, 16, 3,
                                    "../res/tex/castle_brick_07_8k_png/castle_brick_07_rough_8k.png"),
};

int32_t TextureManager::create_item(Texture *item, uint32_t id)
{
    // find index of program shader
    int64_t prog_index = -1;
    for (uint32_t i = 0; i < sizeof(TEXTURE_RESOURCES) / sizeof(const TextureManager::TextureResource *); i++) {
        if (TEXTURE_RESOURCES[i]->id == id) {
            prog_index = i;
            break;
        }
    }
    if (prog_index == -1) {
        nm_log::log(LOG_ERROR, "\"%d\" is not a registered texture id\n", id);

        return EXIT_FAILURE;
    }

    // todo make texture unit flexible once more are needed for a target
    if (TEXTURE_RESOURCES[prog_index]->create_texture(item) == EXIT_FAILURE) {
        nm_log::log(LOG_ERROR, "failed to create texture\n");

        return EXIT_FAILURE;
    }

    nm_log::log(LOG_INFO, "created texture with id \"%d\"\n", id);

    return EXIT_SUCCESS;
}

void TextureManager::delete_item(Texture *item, uint32_t id)
{
    Texture::delete_tex(item);

    nm_log::log(LOG_INFO, "deleted texture with id \"%d\"\n", id);
}

TextureManager::~TextureManager()
{
    for (auto &item : map) {
        delete_item(&item.second.item, item.first);
    }
}