#include "shader_manager.hpp"

// provide a default destructor for the base class
template<>
inline Manager<ShaderProgram>::~Manager<ShaderProgram>() = default;

const ShaderManager::ShaderResource ShaderManager::SHADER_PROGRAM_RESOURCES[] = {
        {
                SHADER_DEFAULT,
                {},
                {"modelMatrix", "viewMatrix", "projectionMatrix"},
                default_vert, &default_vert_len,
                default_frag, &default_frag_len
        },
        {
                SHADER_PHONG,
                {"pos_light", "pos_camera", "color_light"},
                {"modelMatrix", "viewMatrix", "projectionMatrix"},
                phong_vert,   &phong_vert_len,
                phong_frag,   &phong_frag_len
        },
        {
                SHADER_LINES,
                {},
                {"modelMatrix", "viewMatrix", "projectionMatrix"},
                lines_vert,   &lines_vert_len,
                lines_frag,   &lines_frag_len
        },
        {
                SHADER_PBR,
                {},
                {"modelMatrix", "viewMatrix", "projectionMatrix"},
                pbr_vert,     &pbr_vert_len,
                pbr_frag,     &pbr_frag_len
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


