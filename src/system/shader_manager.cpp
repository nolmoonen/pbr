#include "shader_manager.hpp"

// provide a default destructor for the base class
template<>
inline Manager<ShaderProgram>::~Manager<ShaderProgram>() = default;

const ShaderManager::ShaderResource ShaderManager::SHADER_PROGRAM_RESOURCES[] = {
        {
                .id=SHADER_DEFAULT,
                .vec3_ids={},
                .mat4_ids={"modelMatrix", "viewMatrix", "projectionMatrix"},
                .vert_text=default_vert, .vert_len=&default_vert_len,
                .frag_text=default_frag, .frag_len=&default_frag_len
        },
        {
                .id=SHADER_INSTANCED,
                .vec3_ids={},
                .mat4_ids={"modelMatrix", "viewMatrix", "projectionMatrix"},
                .vert_text=instance_vert, .vert_len=&instance_vert_len,
                .frag_text=default_frag, .frag_len=&default_frag_len
        },
        {
                .id=SHADER_PHONG,
                .vec3_ids={"pos_light", "pos_camera", "color_light"},
                .mat4_ids={"modelMatrix", "viewMatrix", "projectionMatrix"},
                .vert_text=phong_vert, .vert_len=&phong_vert_len,
                .frag_text=phong_frag, .frag_len=&phong_frag_len
        },
        {
                .id=SHADER_LINES,
                .vec3_ids={},
                .mat4_ids={"modelMatrix", "viewMatrix", "projectionMatrix"},
                .vert_text=lines_vert, .vert_len=&lines_vert_len,
                .frag_text=lines_frag, .frag_len=&lines_frag_len
        }
};

int32_t ShaderManager::create_item(ShaderProgram *item, uint32_t id)
{
    // find index of program shader
    int64_t prog_index = -1;
    for (uint32_t i = 0; i < sizeof(SHADER_PROGRAM_RESOURCES) / sizeof(ShaderResource); i++) {
        if (SHADER_PROGRAM_RESOURCES[i].id == id) {
            prog_index = i;
            break;
        }
    }
    if (prog_index == -1) {
        nm_log::log(LOG_ERROR, "\"%d\" is not a registered shader program id\n", id);

        return EXIT_FAILURE;
    }

    if (ShaderProgram::create_shader_program(
            item,
            SHADER_PROGRAM_RESOURCES[prog_index].vert_text, *SHADER_PROGRAM_RESOURCES[prog_index].vert_len,
            SHADER_PROGRAM_RESOURCES[prog_index].frag_text, *SHADER_PROGRAM_RESOURCES[prog_index].frag_len
    ) == EXIT_FAILURE) {
        nm_log::log(LOG_ERROR, "failed to create shader program\n");

        return EXIT_FAILURE;
    }

    nm_log::log(LOG_INFO, "created shader program with id \"%d\"\n", id);

    return EXIT_SUCCESS;
}

void ShaderManager::delete_item(ShaderProgram *item, uint32_t id)
{
    ShaderProgram::delete_shader_program(item);

    nm_log::log(LOG_INFO, "deleted shader program with id \"%d\"\n", id);
}

ShaderManager::~ShaderManager()
{
    for (auto &item : map) {
        delete_item(&item.second.item, item.first);
    }
}


