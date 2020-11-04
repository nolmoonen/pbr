#ifndef SYSTEM_SHADER_MANAGER_HPP
#define SYSTEM_SHADER_MANAGER_HPP

#include <cstdint>
#include <vector>

#include "manager.hpp"
#include "../opengl/shader.hpp"

enum ShaderType {
    SHADER_DEFAULT,
    SHADER_PHONG,
    SHADER_LINES,
    SHADER_PBR,
    SHADER_EQUIRECTANGULAR_MAP,
    SHADER_SKYBOX,
    SHADER_IRRADIANCE_MAP,
    SHADER_PRE_FILTER_MAP,
    SHADER_BRDF
};

class ShaderManager : public Manager<ShaderProgram> {
private:
    struct ShaderResource {
        /** Pointers to extern embedded data. */
        const char *vert_text;
        const size_t *vert_len;
        const char *frag_text;
        const size_t *frag_len;
    };

    /** Array to obtain the desired data using an id. */
    static const std::map<uint32_t, ShaderResource> SHADER_PROGRAM_RESOURCES;

    int32_t create_item(ShaderProgram **item, uint32_t id) override;

    void delete_item(ShaderProgram **item, uint32_t id) override;

    /** Non-virtual member function which may be called from destructor. */
    static void delete_item_self(ShaderProgram **item, uint32_t id);

public:
    ~ShaderManager() override;
};

#endif //SYSTEM_SHADER_MANAGER_HPP
