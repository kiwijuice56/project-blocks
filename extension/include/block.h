
#pragma once

#include <godot_cpp/classes/resource.hpp>

namespace godot {

class Block : public Resource {
	GDCLASS(Block, Resource)

private:
    int64_t id = 0;

protected:
	static void _bind_methods();

    void simulate(int64_t state);

    int64_t get_id() const;
    void set_id(int64_t new_id);

public:
	Block();
	~Block();
};

}