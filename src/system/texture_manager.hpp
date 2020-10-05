#ifndef SYSTEM_TEXTURE_MANAGER_HPP
#define SYSTEM_TEXTURE_MANAGER_HPP

#include <cstdint>

#include "opengl.hpp"
#include "manager.hpp"

#define TEXTURE_TEST 0
#define TEXTURE_BRICK_DIFF 1

/** Specified manually based on identifiers in CMakeLists. */
extern const char test_png[];
extern const size_t test_png_len;

extern const char brick_diff_png[];
extern const size_t brick_diff_png_len;

class TextureManager : public Manager<Texture> {
private:
    // todo find better solution and combine with ShaderResource?
    typedef struct {
        uint32_t id;
        uint32_t channels;  // number of channels in the image (for example 3 if RGB)
        uint32_t bit_depth; // number of bits per channel (8 bits is common)
        /** Pointers to extern embedded data. */
        const char *text;
        const size_t *len;
    } TextureResource;

    /** Array to obtain the desired data using an id. */
    static const TextureResource TEXTURE_RESOURCES[];

    int32_t create_item(Texture *item, uint32_t id) override;

    void delete_item(Texture *item, uint32_t id) override;

public:
    ~TextureManager() override;
};

#endif //SYSTEM_TEXTURE_MANAGER_HPP
