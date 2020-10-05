#ifndef SYSTEM_SHADER_MANAGER_HPP
#define SYSTEM_SHADER_MANAGER_HPP

#include <cstdint>
#include <vector>

#include "opengl.hpp"
#include "manager.hpp"

// todo make enum?
#define SHADER_DEFAULT 0
#define SHADER_INSTANCED 1
#define SHADER_PHONG 2
#define SHADER_LINES 3

/** Specified manually based on identifiers in CMakeLists. */
extern const char default_vert[];
extern const size_t default_vert_len;

extern const char default_frag[];
extern const size_t default_frag_len;

extern const char instance_vert[];
extern const size_t instance_vert_len;

extern const char phong_vert[];
extern const size_t phong_vert_len;

extern const char phong_frag[];
extern const size_t phong_frag_len;

extern const char lines_frag[];
extern const size_t lines_vert_len;

extern const char lines_vert[];
extern const size_t lines_frag_len;

class ShaderManager : public Manager<ShaderProgram> {
private:
    // todo find better solution and combine with TextureResource?
    typedef struct {
        uint32_t id;
        // todo currently not used (can be used for checking)
        const std::vector<std::string> vec3_ids;
        const std::vector<std::string> mat4_ids;

        /** Pointers to extern embedded data. */
        const char *vert_text;
        const size_t *vert_len;
        const char *frag_text;
        const size_t *frag_len;
    } ShaderResource;

    /** Array to obtain the desired data using an id. */
    static const ShaderResource SHADER_PROGRAM_RESOURCES[];

    int32_t create_item(ShaderProgram *item, uint32_t id) override;

    void delete_item(ShaderProgram *item, uint32_t id) override;

public:
    ~ShaderManager() override;
};

#endif //SYSTEM_SHADER_MANAGER_HPP
