#include "material.hpp"
#include "manager/texture_manager.hpp"
#include "opengl/texture.hpp"

const std::map<uint32_t, Material *> Material::MATERIALS = {
        {MATERIAL_BRICK_1K,  new Material(
                TEXTURE_BRICK_1_DIFF,
                TEXTURE_BRICK_1_NORM,
                TEXTURE_BRICK_1_AO,
                TEXTURE_BRICK_1_ROUGH,
                TEXTURE_BRICK_1_DISP)},
        {MATERIAL_BRICK_2K,  new Material(
                TEXTURE_BRICK_2_DIFF,
                TEXTURE_BRICK_2_NORM,
                TEXTURE_BRICK_2_AO,
                TEXTURE_BRICK_2_ROUGH,
                TEXTURE_BRICK_2_DISP)},
        {MATERIAL_BRICK_4K,  new Material(
                TEXTURE_BRICK_4_DIFF,
                TEXTURE_BRICK_4_NORM,
                TEXTURE_BRICK_4_AO,
                TEXTURE_BRICK_4_ROUGH,
                TEXTURE_BRICK_4_DISP)},
        {MATERIAL_BRICK_8K,  new Material(
                TEXTURE_BRICK_8_DIFF,
                TEXTURE_BRICK_8_NORM,
                TEXTURE_BRICK_8_AO,
                TEXTURE_BRICK_8_ROUGH,
                TEXTURE_BRICK_8_DISP)},
        {MATERIAL_METAL_1K,  new MaterialSpecular(
                TEXTURE_METAL_1_DIFF,
                TEXTURE_METAL_1_NORM,
                TEXTURE_METAL_1_AO,
                TEXTURE_METAL_1_ROUGH,
                TEXTURE_METAL_1_DISP,
                TEXTURE_METAL_1_SPEC)},
        {MATERIAL_MARBLE_1K, new MaterialSpecular(
                TEXTURE_MARBLE_1_DIFF,
                TEXTURE_MARBLE_1_NORM,
                TEXTURE_MARBLE_1_AO,
                TEXTURE_MARBLE_1_ROUGH,
                TEXTURE_MARBLE_1_DISP,
                TEXTURE_MARBLE_1_SPEC)},
        {MATERIAL_DENIM_1K,  new Material(
                TEXTURE_DENIM_1_DIFF,
                TEXTURE_DENIM_1_NORM,
                TEXTURE_DENIM_1_AO,
                TEXTURE_DENIM_1_ROUGH,
                TEXTURE_DENIM_1_DISP)}
};

int Material::get_material_by_id(uint32_t id, Material **material)
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

void Material::set(ShaderProgram *program, TextureManager *manager)
{
    ShaderProgram::set_int(program, "texture_diff",
                           (signed) manager->get(diffuse)->texture_unit - GL_TEXTURE0);
    ShaderProgram::set_int(program, "texture_norm",
                           (signed) manager->get(normal)->texture_unit - GL_TEXTURE0);
    ShaderProgram::set_int(program, "texture_ao",
                           (signed) manager->get(ambient_occlusion)->texture_unit - GL_TEXTURE0);
    ShaderProgram::set_int(program, "texture_rough",
                           (signed) manager->get(roughness)->texture_unit - GL_TEXTURE0);
    ShaderProgram::set_int(program, "texture_disp",
                           (signed) manager->get(displacement)->texture_unit - GL_TEXTURE0);

    ShaderProgram::set_int(program, "has_spec", 0);
}

Material::Material(
        uint32_t diffuse, uint32_t normal, uint32_t ambient_occlusion, uint32_t roughness, uint32_t displacement
) :
        diffuse(diffuse), normal(normal), ambient_occlusion(ambient_occlusion), roughness(roughness),
        displacement(displacement)
{}

void Material::bind(TextureManager *manager)
{
    Texture::bind_tex(manager->get(diffuse));
    Texture::bind_tex(manager->get(normal));
    Texture::bind_tex(manager->get(ambient_occlusion));
    Texture::bind_tex(manager->get(roughness));
    Texture::bind_tex(manager->get(displacement));
}

void Material::unbind(TextureManager *manager)
{
    Texture::unbind_tex(manager->get(displacement));
    Texture::unbind_tex(manager->get(roughness));
    Texture::unbind_tex(manager->get(ambient_occlusion));
    Texture::unbind_tex(manager->get(normal));
    Texture::unbind_tex(manager->get(diffuse));
}

MaterialSpecular::MaterialSpecular(
        uint32_t diffuse, uint32_t normal, uint32_t ambient_occlusion, uint32_t roughness, uint32_t displacement,
        uint32_t specular
) :
        Material(diffuse, normal, ambient_occlusion, roughness, displacement), specular(specular)
{}

void MaterialSpecular::set(ShaderProgram *program, TextureManager *manager)
{
    Material::set(program, manager);

    ShaderProgram::set_int(program, "has_spec", 1);
    ShaderProgram::set_int(program, "texture_spec",
                           (signed) manager->get(specular)->texture_unit - GL_TEXTURE0);
}

void MaterialSpecular::bind(TextureManager *manager)
{
    Material::bind(manager);
    Texture::bind_tex(manager->get(specular));
}

void MaterialSpecular::unbind(TextureManager *manager)
{
    Material::unbind(manager);
    Texture::unbind_tex(manager->get(specular));
}