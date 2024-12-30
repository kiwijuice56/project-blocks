
#pragma once

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/ref_counted.hpp>

namespace godot {

class ItemState : public Resource {
	GDCLASS(ItemState, Resource);

private:

protected:
    uint32_t id = 0;
    uint32_t count = 1;

	static void _bind_methods();

    void simulate(uint64_t state);

public:
	ItemState();
	~ItemState();

    uint32_t get_id() const;
    void set_id(uint32_t new_id);

    uint32_t get_count() const;
    void set_count(uint32_t new_count);
};

}