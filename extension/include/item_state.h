
#pragma once

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/ref_counted.hpp>

namespace godot {

class ItemState : public Resource {
	GDCLASS(ItemState, Resource);

protected:
	static void _bind_methods();

public:
    uint32_t id = 0;
    uint32_t count = 1;

	ItemState();
	~ItemState();

    uint32_t get_id() const;
    void set_id(uint32_t new_id);

    uint32_t get_count() const;
    void set_count(uint32_t new_count);
};

}