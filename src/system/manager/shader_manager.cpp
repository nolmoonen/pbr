#include "shader_manager.hpp"
#include "embedded.hpp"

// provide a default destructor for the base class
template<>
inline Manager<ShaderProgram>::~Manager<ShaderProgram>() = default;

const std::map<uint32_t, ShaderManager::ShaderResource> ShaderManager::SHADER_PROGRAM_RESOURCES = {
        {SHADER_DEFAULT, {default_vert, &default_vert_len, default_frag, &default_frag_len}},
        {SHADER_PHONG,   {phong_vert,   &phong_vert_len,   phong_frag,   &phong_frag_len}},
        {SHADER_LINES,   {lines_vert,   &lines_vert_len,   lines_frag,   &lines_frag_len}},
        {SHADER_PBR,     {pbr_vert,     &pbr_vert_len,     pbr_frag,     &pbr_frag_len}}};

int32_t ShaderManager::create_item(ShaderProgram **item, uint32_t id)
{
    // find index of program shader
    auto entry = SHADER_PROGRAM_RESOURCES.find(id);
    if (entry == SHADER_PROGRAM_RESOURCES.end()) {
        nm_log::log(LOG_ERROR, "\"%d\" is not a registered shader program id\n", id);

        return EXIT_FAILURE;
    }

    // else, create from function pointer
    *item = new ShaderProgram();
    int32_t rval = ShaderProgram::create_shader_program(
            *item,
            entry->second.vert_text, *entry->second.vert_len,
            entry->second.frag_text, *entry->second.frag_len);
    if (rval == EXIT_FAILURE) {
        nm_log::log(LOG_ERROR, "failed to create shader program\n");

        delete *item;

        return EXIT_FAILURE;
    }

    nm_log::log(LOG_INFO, "created shader program with id \"%d\"\n", id);

    return EXIT_SUCCESS;
}

void ShaderManager::delete_item(ShaderProgram **item, uint32_t id)
{
    delete_item_self(item, id);
}

void ShaderManager::delete_item_self(ShaderProgram **item, uint32_t id)
{
    ShaderProgram::delete_shader_program(*item); // delete the created data associated with this handle
    delete (*item);                              // delete the handle itself

    nm_log::log(LOG_INFO, "deleted shader program with id \"%d\"\n", id);
}

ShaderManager::~ShaderManager()
{
    for (auto &item : map) {
        delete_item_self(&item.second.item, item.first);
    }
}