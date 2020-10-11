#ifndef SYSTEM_MESH_MANAGER_HPP
#define SYSTEM_MESH_MANAGER_HPP

#include "manager.hpp"
#include "../opengl/primitive/primitive.hpp"

enum PrimitiveTypes {
    /** Primitives */
    PRIMITIVE_CONE,
    PRIMITIVE_CYLINDER,
    /** FullPrimitives */
    PRIMITIVE_SKYBOX,
    PRIMITIVE_CUBE,
    PRIMITIVE_SPHERE,
    /** LinePrimitives */
    PRIMITIVE_COORDINATE_SYSTEM,
    PRIMITIVE_SPHERE_NORMALS
};

class PrimitiveManager : public Manager<Primitive> {
private:
    struct PrimitiveResource {
        Primitive *(*create_function)();
    };

    /** Array to obtain the desired data using an id. */
    static const std::map<uint32_t, PrimitiveResource> PRIMITIVE_RESOURCES;

    int32_t create_item(Primitive **item, uint32_t id) override;

    void delete_item(Primitive **item, uint32_t id) override;

    /** Non-virtual member function which may be called from destructor. */
    static void delete_item_self(Primitive **item, uint32_t id);

public:
    ~PrimitiveManager() override;
};

#endif //SYSTEM_MESH_MANAGER_HPP
