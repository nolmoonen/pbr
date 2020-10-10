#ifndef PBR_MATERIAL_HPP
#define PBR_MATERIAL_HPP

#include <cstdint>

#define MATERIAL_BRICK_1K 0
#define MATERIAL_BRICK_2K 1
#define MATERIAL_BRICK_4K 2
#define MATERIAL_BRICK_8K 3

struct Material {
    uint32_t id;
    uint32_t diffuse;
    uint32_t normal;
    uint32_t ambient_occlusion;
    uint32_t roughness;
    uint32_t displacement;
    float metallic;

    static const Material MATERIALS[];

    static int get_material_by_id(uint32_t id, Material *material);
};

#endif //PBR_MATERIAL_HPP
