#include "material.hpp"
#include "manager/texture_manager.hpp"

const std::map<uint32_t, Material> Material::MATERIALS = {
        {MATERIAL_BRICK_1K, {
                                    TEXTURE_BRICK_1_DIFF,
                                    TEXTURE_BRICK_1_NORM,
                                    TEXTURE_BRICK_1_AO,
                                    TEXTURE_BRICK_1_ROUGH,
                                    TEXTURE_BRICK_1_DISP,
                                    0.f}},
        {MATERIAL_BRICK_2K, {
                                    TEXTURE_BRICK_2_DIFF,
                                    TEXTURE_BRICK_2_NORM,
                                    TEXTURE_BRICK_2_AO,
                                    TEXTURE_BRICK_2_ROUGH,
                                    TEXTURE_BRICK_2_DISP,
                                    0.f}},
        {MATERIAL_BRICK_4K, {
                                    TEXTURE_BRICK_4_DIFF,
                                    TEXTURE_BRICK_4_NORM,
                                    TEXTURE_BRICK_4_AO,
                                    TEXTURE_BRICK_4_ROUGH,
                                    TEXTURE_BRICK_4_DISP,
                                    0.f}},
        {MATERIAL_BRICK_8K, {
                                    TEXTURE_BRICK_8_DIFF,
                                    TEXTURE_BRICK_8_NORM,
                                    TEXTURE_BRICK_8_AO,
                                    TEXTURE_BRICK_8_ROUGH,
                                    TEXTURE_BRICK_8_DISP,
                                    0.f}}
};

int Material::get_material_by_id(uint32_t id, Material *material)
{
    // find index of program shader
    auto entry = MATERIALS.find(id);
    if (entry == MATERIALS.end()) {
        nm_log::log(LOG_ERROR, "\"%d\" is not a registered material id\n", id);

        return EXIT_FAILURE;
    }

    *material = entry->second;

    return EXIT_SUCCESS;
}
