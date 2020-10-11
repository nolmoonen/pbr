#include "primitive_manager.hpp"

#include "../opengl/primitive/full_primitive.hpp"

// provide a default destructor for the base class
template<>
inline Manager<Primitive>::~Manager<Primitive>() = default;

const std::map<uint32_t, PrimitiveManager::PrimitiveResource> PrimitiveManager::PRIMITIVE_RESOURCES = {
        {PRIMITIVE_CONE,   {&Primitive::create_cone}},
        {PRIMITIVE_CUBE,   {&FullPrimitive::create_cube}},
        {PRIMITIVE_SKYBOX, {&FullPrimitive::create_skybox}},
        {PRIMITIVE_SPHERE, {&FullPrimitive::create_sphere}}
};

int32_t PrimitiveManager::create_item(Primitive **item, uint32_t id)
{
    // find index of program shader
    auto entry = PRIMITIVE_RESOURCES.find(id);
    if (entry == PRIMITIVE_RESOURCES.end()) {
        nm_log::log(LOG_ERROR, "\"%d\" is not a registered primitive id\n", id);

        return EXIT_FAILURE;
    }

    // else, create from function pointer
    *item = entry->second.create_function();

    nm_log::log(LOG_INFO, "created primitive with id \"%d\"\n", id);

    return EXIT_SUCCESS;
}

void PrimitiveManager::delete_item(Primitive **item, uint32_t id)
{
    delete_item_self(item, id);
}

void PrimitiveManager::delete_item_self(Primitive **item, uint32_t id)
{
    (*item)->delete_primitive(); // delete the created data associated with this handle
    delete (*item);     // delete the handle itself

    nm_log::log(LOG_INFO, "deleted primitive with id \"%d\"\n", id);
}

PrimitiveManager::~PrimitiveManager()
{
    for (auto &item : map) {
        delete_item_self(&item.second.item, item.first);
    }
}