#include "mesh_manager.hpp"

// provide a default destructor for the base class
template<>
inline Manager<Mesh>::~Manager<Mesh>() = default;

const MeshManager::MeshResource MeshManager::MESH_RESOURCES[] = {
        {MESH_SKYBOX, &Mesh::create_skybox},
        {MESH_CUBE,   &Mesh::create_cube},
        {MESH_SPHERE, &Mesh::create_sphere},
};

int32_t MeshManager::create_item(Mesh *item, uint32_t id)
{
    // find index of program shader
    int64_t prog_index = -1;
    for (uint32_t i = 0; i < sizeof(MESH_RESOURCES) / sizeof(MeshResource); i++) {
        if (MESH_RESOURCES[i].id == id) {
            prog_index = i;
            break;
        }
    }
    if (prog_index == -1) {
        nm_log::log(LOG_ERROR, "\"%d\" is not a registered texture id\n", id);

        return EXIT_FAILURE;
    }

    // else, create from function pointer
    MESH_RESOURCES[prog_index].create_function(item);

    nm_log::log(LOG_INFO, "created mesh with id \"%d\"\n", id);

    return EXIT_SUCCESS;
}

void MeshManager::delete_item(Mesh *item, uint32_t id)
{
    Mesh::delete_mesh(item);

    nm_log::log(LOG_INFO, "deleted mesh with id \"%d\"\n", id);
}

MeshManager::~MeshManager()
{
    for (auto &item : map) {
        delete_item(&item.second.item, item.first);
    }
}
