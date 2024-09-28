
#pragma once

#include "../include/block.h"
#include "../include/chunk.h"

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/material.hpp>

namespace godot {

class World : public Node3D {
	GDCLASS(World, Node3D)

private:
	int64_t instance_radius = 80;
	int64_t unload_radius = 160;

	TypedArray<Block> block_types;
	Ref<Material> block_material;

	Vector3 center;
	Vector3i center_chunk;

	// Vector3 coordinate : Chunk instance mapping
	Dictionary loaded_chunks;

	// Vector3 coordinate : bool mapping
	Dictionary is_chunk_instanced;

	// Vector3 coordinate : bool mapping
	Dictionary is_chunk_in_queue;

	PackedVector3Array generation_queue;

protected:
	static void _bind_methods();

	void generate_chunk(Vector3i coordinate);
	void generate_from_queue(uint64_t n);
	void update_loaded_region();

	bool is_chunk_in_radius(Vector3i coordinate, int64_t radius);

public:
	World();
	~World();

	TypedArray<Block> get_block_types() const;
    void set_block_types(const TypedArray<Block> new_block_types);

	Ref<Material> get_block_material() const;
    void set_block_material(Ref<Material> new_material);

	Vector3 get_center() const;
	void set_center(Vector3 new_center);
};

}