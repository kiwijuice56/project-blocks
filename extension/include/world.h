
#pragma once

#include "../include/block.h"
#include "../include/chunk.h"

#include <godot_cpp/classes/node3d.hpp>

namespace godot {

class World : public Node3D {
	GDCLASS(World, Node3D)

private:
	int64_t load_radius = 4;
	TypedArray<Block> block_types;

	Vector3 center;
	Vector3i center_chunk;

	// std::unordered_map<uint64_t, Chunk*> stored_chunks;
	Dictionary stored_chunks;

protected:
	static void _bind_methods();

	void generate_chunk(Vector3i coordinate);
	void update_loaded_region();

public:
	World();
	~World();

	TypedArray<Block> get_block_types() const;
    void set_block_types(const TypedArray<Block> new_block_types);

	Vector3 get_center() const;
	void set_center(Vector3 new_center);
};

}