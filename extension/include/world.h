
#pragma once

#include "../include/block.h"

#include <godot_cpp/classes/node3d.hpp>

namespace godot {

class World : public Node3D {
	GDCLASS(World, Node3D)

private:
	TypedArray<Block> block_types;

protected:
	static void _bind_methods();

	void generate_chunk(int64_t x, int64_t y);

public:
	World();
	~World();

	TypedArray<Block> get_block_types() const;
    void set_block_types(const TypedArray<Block> new_block_types);
};

}