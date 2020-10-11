#ifndef SYSTEM_TEXTURE_MANAGER_HPP
#define SYSTEM_TEXTURE_MANAGER_HPP

#include <cstdint>

#include "manager.hpp"
#include "../opengl/texture.hpp"

enum TextureType {
    TEXTURE_BRICK_1_DIFF,
    TEXTURE_BRICK_1_NORM,
    TEXTURE_BRICK_1_AO,
    TEXTURE_BRICK_1_ROUGH,
    TEXTURE_BRICK_1_DISP,

    TEXTURE_BRICK_2_DIFF,
    TEXTURE_BRICK_2_NORM,
    TEXTURE_BRICK_2_AO,
    TEXTURE_BRICK_2_ROUGH,
    TEXTURE_BRICK_2_DISP,

    TEXTURE_BRICK_4_DIFF,
    TEXTURE_BRICK_4_NORM,
    TEXTURE_BRICK_4_AO,
    TEXTURE_BRICK_4_ROUGH,
    TEXTURE_BRICK_4_DISP,

    TEXTURE_BRICK_8_DIFF,
    TEXTURE_BRICK_8_NORM,
    TEXTURE_BRICK_8_AO,
    TEXTURE_BRICK_8_ROUGH,
    TEXTURE_BRICK_8_DISP,

    TEXTURE_TEST
};

class TextureManager : public Manager<Texture> {
private:
    struct TextureResource {
        uint32_t channels;     // number of channels in the image (for example 3 if RGB)
        uint32_t bit_depth;    // number of bits per channel (8 bits is common)
        uint32_t texture_unit; // to which opengl texture unit it maps

        TextureResource(uint32_t channels, uint32_t bit_depth, uint32_t texture_unit);

        virtual int create_texture(Texture *texture) const = 0;
    };

    struct TextureResourceFromMemory : public TextureResource {
        /** Pointers to extern embedded data. */
        const char *text;
        const size_t *len;

        TextureResourceFromMemory(
                uint32_t channels, uint32_t bit_depth, uint32_t texture_unit,
                const char *text, const size_t *len);

        int create_texture(Texture *texture) const override;
    };

    struct TextureResourceFromFile : public TextureResource {
        /** File name. */
        const char *file_name;

        TextureResourceFromFile(
                uint32_t channels, uint32_t bit_depth, uint32_t texture_unit,
                const char *file_name);

        int create_texture(Texture *texture) const override;
    };

    /** Array to obtain the desired data using an id. */
    static const std::map<uint32_t, TextureResource *> TEXTURE_RESOURCES;

    int32_t create_item(Texture **item, uint32_t id) override;

    void delete_item(Texture **item, uint32_t id) override;

    /** Non-virtual member function which may be called from destructor. */
    static void delete_item_self(Texture **item, uint32_t id);

public:
    ~TextureManager() override;
};

#endif //SYSTEM_TEXTURE_MANAGER_HPP
