#ifndef SYSTEM_TEXTURE_MANAGER_HPP
#define SYSTEM_TEXTURE_MANAGER_HPP

#include <cstdint>

#include "manager.hpp"

enum TextureType {
    TEXTURE_TEST,

    BRDF_LUT,

    TEXTURE_STUDIO_HDR,
    CUBEMAP_STUDIO,
    CUBEMAP_STUDIO_IRRADIANCE,
    CUBEMAP_STUDIO_PRE_FILTER,

    TEXTURE_MOONLESS_GOLF_HDR,
    CUBEMAP_MOONLESS_GOLF,
    CUBEMAP_MOONLESS_GOLF_IRRADIANCE,
    CUBEMAP_MOONLESS_GOLF_PRE_FILTER,

    TEXTURE_NOON_GRASS_HDR,
    CUBEMAP_NOON_GRASS,
    CUBEMAP_NOON_GRASS_IRRADIANCE,
    CUBEMAP_NOON_GRASS_PRE_FILTER,

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

    TEXTURE_METAL_1_DIFF,
    TEXTURE_METAL_1_NORM,
    TEXTURE_METAL_1_AO,
    TEXTURE_METAL_1_ROUGH,
    TEXTURE_METAL_1_DISP,
    TEXTURE_METAL_1_SPEC,

    TEXTURE_MARBLE_1_DIFF,
    TEXTURE_MARBLE_1_NORM,
    TEXTURE_MARBLE_1_AO,
    TEXTURE_MARBLE_1_ROUGH,
    TEXTURE_MARBLE_1_DISP,
    TEXTURE_MARBLE_1_SPEC,

    TEXTURE_DENIM_1_DIFF,
    TEXTURE_DENIM_1_NORM,
    TEXTURE_DENIM_1_AO,
    TEXTURE_DENIM_1_ROUGH,
    TEXTURE_DENIM_1_DISP
};

class Texture;

class TextureManager : public Manager<Texture> {
public:
    enum ChannelType {
        INTEGER, FLOATING_POINT
    };
    enum WrapType {
        CLAMP, REPEAT
    };
private:
    struct TextureResource {
        uint32_t channels;     // number of channels in the image (for example 3 if RGB)
        uint32_t bit_depth;    // number of bits per channel (8 bits is common)
        uint32_t texture_unit; // to which opengl texture unit it maps
        ChannelType type;      // what the type of the channel data is
        WrapType wrap_type;    // texture parameter

        TextureResource(
                uint32_t channels, uint32_t bit_depth, uint32_t texture_unit, ChannelType type, WrapType wrap_type);

        virtual int create_texture(Texture *texture) const = 0;
    };

    struct TextureResourceFromMemory : public TextureResource {
        /** Pointers to extern embedded data. */
        const char *text;
        const size_t *len;

        TextureResourceFromMemory(
                uint32_t channels, uint32_t bit_depth, uint32_t texture_unit, ChannelType type, WrapType wrap_type,
                const char *text, const size_t *len);

        int create_texture(Texture *texture) const override;
    };

    struct TextureResourceFromFile : public TextureResource {
        /** File name. */
        const char *file_name;

        TextureResourceFromFile(
                uint32_t channels, uint32_t bit_depth, uint32_t texture_unit, ChannelType type, WrapType wrap_type,
                const char *file_name);

        int create_texture(Texture *texture) const override;
    };

    struct TextureResourceFromTextureResource : public TextureResource {
        TextureType texture_type;

        /** First texture is the one to create, second is the one to use it, last parameter is texture unit. */
        int (*create_function)(Texture *, Texture *, uint32_t);

        TextureResourceFromTextureResource(
                TextureType texture_type, uint32_t texture_unit,
                int (*create_function)(Texture *, Texture *, uint32_t));

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
