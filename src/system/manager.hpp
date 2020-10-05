#ifndef SYSTEM_MANAGER_HPP
#define SYSTEM_MANAGER_HPP

#include <cstdint>
#include <ctime>
#include <cstdlib>
#include <array>
#include <vector>
#include <map>

#include "../util/nm_log.hpp"

// todo build in a warning for high memory usage?
template<typename T>
class Manager {
private:
    struct Item {
        /** Actual item held in the struct. */
        T item;
        /** Whether it was used last frame. */
        bool used;

        Item(T item, bool used) : item(item), used(used)
        {}
    };

    virtual int32_t create_item(T *item, uint32_t id) = 0;

    virtual void delete_item(T *item, uint32_t id) = 0;

protected:

    /** Map of all items, indexed by id. */
    std::map<uint32_t, Item> map;

public:
    /** Pure virtual, to force implementers to call {delete_item} on all remaining items in {map}. */
    virtual ~Manager() = 0;

    /**
     * Returns a pointer to the requested object indicated by {id}.
     * Remains valid until the next call to {get} or {make_space}. */
    // todo error handling
    T *get(uint32_t id)
    {
        // try to find in buffer
        auto item = map.find(id);
        if (item != map.end()) {
            item->second.used = true;
            return &(item->second.item);
        }

        // create new
        T t;
        if (create_item(&t, id) != EXIT_SUCCESS) {
            nm_log::log(LOG_ERROR, "manager could not create item!\n");
        }

        map.insert(std::make_pair(id, Item(t, true)));

        return &map.find(id)->second.item;
    }

    /** Deletes and removes from the map all items that were not used in the last frame. */
    void make_space()
    {
        // https://stackoverflow.com/questions/8234779/how-to-remove-from-a-map-while-iterating-it
        for (auto it = map.begin(); it != map.end() /* not hoisted */; /* no increment */) {
            if (!it->second.used) {
                // deallocate all items not used
                delete_item(&it->second.item, it->first);
                // remove from map
                it = map.erase(it);
            } else {
                // set status of all remaining items to not used
                it->second.used = false;
                ++it;
            }
        }
    };
};

#endif //SYSTEM_MANAGER_HPP