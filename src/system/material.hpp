#ifndef PBR_MATERIAL_HPP
#define PBR_MATERIAL_HPP

#include <cstdint>
#include <map>

#include "opengl/shader.hpp"
#include "manager/texture_manager.hpp"

struct Material {
    enum MaterialType {
        MATERIAL_BRICK_1K,
        MATERIAL_BRICK_2K,
        MATERIAL_BRICK_4K,
        MATERIAL_BRICK_8K,
        MATERIAL_METAL_1K,
        MATERIAL_MARBLE_1K,
        MATERIAL_DENIM_1K
    };

    uint32_t diffuse;
    uint32_t normal;
    uint32_t ambient_occlusion;
    uint32_t roughness;
    uint32_t displacement;

    Material(uint32_t diffuse, uint32_t normal, uint32_t ambient_occlusion, uint32_t roughness, uint32_t displacement);

    static const std::map<uint32_t, Material *> MATERIALS;

    static int get_material_by_id(uint32_t id, Material **material);

    virtual void set(ShaderProgram *program, TextureManager *manager);

    virtual void bind(TextureManager *manager);

    virtual void unbind(TextureManager *manager);
};

struct MaterialSpecular : public Material {
    uint32_t specular;

    MaterialSpecular(uint32_t diffuse, uint32_t normal, uint32_t ambient_occlusion, uint32_t roughness,
                     uint32_t displacement, uint32_t specular);

    void set(ShaderProgram *program, TextureManager *manager) override;

    void bind(TextureManager *manager) override;

    void unbind(TextureManager *manager) override;
};

#endif //PBR_MATERIAL_HPP
