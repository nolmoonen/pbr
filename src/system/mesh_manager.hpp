#ifndef SYSTEM_MESH_MANAGER_HPP
#define SYSTEM_MESH_MANAGER_HPP

#include "manager.hpp"
#include "opengl/mesh.hpp"

#define MESH_SKYBOX 0
#define MESH_CUBE 1
#define MESH_SPHERE 2

class MeshManager : public Manager<Mesh> {
private:
    // todo find better solution and combine with ShaderResource?
    typedef struct {
        uint32_t id;
        void (*create_function)(Mesh *);
    } MeshResource;

    /** Array to obtain the desired data using an id. */
    static const MeshResource MESH_RESOURCES[];

    int32_t create_item(Mesh *item, uint32_t id) override;

    void delete_item(Mesh *item, uint32_t id) override;

public:
    ~MeshManager() override;
};

#endif //SYSTEM_MESH_MANAGER_HPP
