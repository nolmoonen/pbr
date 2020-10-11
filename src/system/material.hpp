#ifndef PBR_MATERIAL_HPP
#define PBR_MATERIAL_HPP

#include <cstdint>
#include <map>

struct Material {
    enum MaterialType {
        MATERIAL_BRICK_1K,
        MATERIAL_BRICK_2K,
        MATERIAL_BRICK_4K,
        MATERIAL_BRICK_8K
    };

    uint32_t diffuse;
    uint32_t normal;
    uint32_t ambient_occlusion;
    uint32_t roughness;
    uint32_t displacement;
    float metallic;

    static const std::map<uint32_t, Material> MATERIALS;

    static int get_material_by_id(uint32_t id, Material *material);
};

#endif //PBR_MATERIAL_HPP
