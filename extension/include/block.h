
#pragma once

#include <godot_cpp/classes/resource.hpp>

namespace godot {

class Block : public Resource {
	GDCLASS(Block, Resource)

private:
    uint8_t id = 0;

protected:
	static void _bind_methods();

    void simulate(uint64_t state);

    uint8_t get_id() const;
    void set_id(uint8_t new_id);

public:
	Block();
	~Block();
};

}