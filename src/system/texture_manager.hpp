#ifndef SYSTEM_TEXTURE_MANAGER_HPP
#define SYSTEM_TEXTURE_MANAGER_HPP

#include <cstdint>

#include "manager.hpp"
#include "opengl/texture.hpp"

/** 0x000 and 0x010 reserved for TEXTURE_BRICK */

#define TEXTURE_BRICK_1_DIFF  0x000
#define TEXTURE_BRICK_1_NORM  0x001
#define TEXTURE_BRICK_1_AO    0x002
#define TEXTURE_BRICK_1_ROUGH 0x003
#define TEXTURE_BRICK_1_DISP  0x004

#define TEXTURE_BRICK_2_DIFF  0x005
#define TEXTURE_BRICK_2_NORM  0x006
#define TEXTURE_BRICK_2_AO    0x007
#define TEXTURE_BRICK_2_ROUGH 0x008
#define TEXTURE_BRICK_2_DISP  0x009

#define TEXTURE_BRICK_4_DIFF  0x00A
#define TEXTURE_BRICK_4_NORM  0x00B
#define TEXTURE_BRICK_4_AO    0x00C
#define TEXTURE_BRICK_4_ROUGH 0x00D
#define TEXTURE_BRICK_4_DISP  0x00E

#define TEXTURE_BRICK_8_DIFF  0x00F
#define TEXTURE_BRICK_8_NORM  0x010
#define TEXTURE_BRICK_8_AO    0x011
#define TEXTURE_BRICK_8_ROUGH 0x012
#define TEXTURE_BRICK_8_DISP  0x013

#define TEXTURE_TEST          0x020

/** Specified manually based on identifiers in CMakeLists. */
extern const char test_png[];
extern const size_t test_png_len;

extern const char brick_diff_png[];
extern const size_t brick_diff_png_len;

extern const char brick_norm_png[];
extern const size_t brick_norm_png_len;

extern const char brick_ao_png[];
extern const size_t brick_ao_png_len;

extern const char brick_rough_png[];
extern const size_t brick_rough_png_len;

extern const char brick_disp_png[];
extern const size_t brick_disp_png_len;

class TextureManager : public Manager<Texture> {
private:
    struct TextureResource {
        uint32_t id;
        uint32_t channels;     // number of channels in the image (for example 3 if RGB)
        uint32_t bit_depth;    // number of bits per channel (8 bits is common)
        uint32_t texture_unit; // to which opengl texture unit it maps

        TextureResource(uint32_t id, uint32_t channels, uint32_t bit_depth, uint32_t texture_unit);

        virtual int create_texture(Texture *texture) const = 0;
    };

    struct TextureResourceFromMemory : public TextureResource {
        /** Pointers to extern embedded data. */
        const char *text;
        const size_t *len;

        TextureResourceFromMemory(
                uint32_t id, uint32_t channels, uint32_t bit_depth, uint32_t texture_unit,
                const char *text, const size_t *len);

        int create_texture(Texture *texture) const override;
    };

    struct TextureResourceFromFile : public TextureResource {
        /** File name. */
        const char *file_name;

        TextureResourceFromFile(
                uint32_t id, uint32_t channels, uint32_t bit_depth, uint32_t texture_unit,
                const char *file_name);

        int create_texture(Texture *texture) const override;
    };

    /** Array to obtain the desired data using an id. */
    static const TextureResource *TEXTURE_RESOURCES[];

    int32_t create_item(Texture *item, uint32_t id) override;

    void delete_item(Texture *item, uint32_t id) override;

public:
    ~TextureManager() override;
};

#endif //SYSTEM_TEXTURE_MANAGER_HPP
