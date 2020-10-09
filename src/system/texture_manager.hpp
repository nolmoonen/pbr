#ifndef SYSTEM_TEXTURE_MANAGER_HPP
#define SYSTEM_TEXTURE_MANAGER_HPP

#include <cstdint>

#include "opengl.hpp"
#include "manager.hpp"

#define TEXTURE_TEST 0

#define TEXTURE_BRICK_DIFF  1
#define TEXTURE_BRICK_NORM  2
#define TEXTURE_BRICK_AO    3
#define TEXTURE_BRICK_ROUGH 4

#define TEXTURE_BRICK_8_DIFF  5
#define TEXTURE_BRICK_8_NORM  6
#define TEXTURE_BRICK_8_AO    7
#define TEXTURE_BRICK_8_ROUGH 8

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
