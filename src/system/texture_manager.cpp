#include "texture_manager.hpp"

// provide a default destructor for the base class
template<>
inline Manager<Texture>::~Manager<Texture>() = default;

const TextureManager::TextureResource TextureManager::TEXTURE_RESOURCES[] = {
        {TEXTURE_TEST,        4, 8,  0, test_png,        &test_png_len},
        {TEXTURE_BRICK_DIFF,  4, 16, 0, brick_diff_png,  &brick_diff_png_len},
        {TEXTURE_BRICK_NORM,  4, 16, 1, brick_norm_png,  &brick_norm_png_len},
        {TEXTURE_BRICK_AO,    2, 16, 2, brick_ao_png,    &brick_ao_png_len},
        {TEXTURE_BRICK_ROUGH, 2, 16, 3, brick_rough_png, &brick_rough_png_len},
};

int32_t TextureManager::create_item(Texture *item, uint32_t id)
{
    // find index of program shader
    int64_t prog_index = -1;
    for (uint32_t i = 0; i < sizeof(TEXTURE_RESOURCES) / sizeof(TextureResource); i++) {
        if (TEXTURE_RESOURCES[i].id == id) {
            prog_index = i;
            break;
        }
    }
    if (prog_index == -1) {
        nm_log::log(LOG_ERROR, "\"%d\" is not a registered texture id\n", id);

        return EXIT_FAILURE;
    }

    // todo make texture unit flexible once more are needed for a target
    if (Texture::create_tex_from_mem(
            item,
            TEXTURE_RESOURCES[prog_index].text, *TEXTURE_RESOURCES[prog_index].len,
            TEXTURE_RESOURCES[prog_index].channels, TEXTURE_RESOURCES[prog_index].bit_depth,
            TEXTURE_RESOURCES[prog_index].texture_unit
    ) == EXIT_FAILURE) {
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
