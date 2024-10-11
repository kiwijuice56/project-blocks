
#pragma once

#include "../include/block.h"
#include "../include/chunk.h"

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/material.hpp>
#include <godot_cpp/classes/noise_texture2d.hpp>
#include <godot_cpp/classes/worker_thread_pool.hpp>
#include <godot_cpp/classes/thread.hpp>
#include <godot_cpp/classes/engine.hpp>

namespace godot {

class World : public Node3D {
	GDCLASS(World, Node3D)

private:
	int64_t instance_radius = 128;

	// Resources
	TypedArray<Block> block_types;
	Ref<Material> block_material;
	Ref<NoiseTexture2D> main_noise_texture;

	// The center chunk's position
	Vector3i center_chunk;

	// Stores references to all chunk node instances
	std::vector<Chunk*> all_chunks;

	// Stores coordinates (Vector3i) of loaded chunks
	Dictionary is_chunk_loaded;

	// Stores (Vector3i : Chunk) mapping for easy access
	Dictionary chunk_map;

	// Stores block data of modified chunks (Vector3i : PackedByteArray)
	Dictionary chunk_data;

	// Used to access chunks that need to be initialized
	std::vector<Chunk*> initiliazation_queue;
	std::vector<Vector3i> initiliazation_queue_positions;

	// Multithreading state
	uint64_t task_id = 0;
	bool has_task = false;

protected:
	static void _bind_methods();

	void regenerate_chunks();
	void initialize_chunk(uint64_t index);
	void update_loaded_region();

	bool is_chunk_in_radius(Vector3i coordinate, int64_t radius);

public:
	World();
	~World();

	void initialize();

	// Used to set the loaded region of the world;
	// new_center is usually the player's position
	void set_loaded_region_center(Vector3 new_center);

	// Checks whether the given position is in a loaded chunk or not
	bool is_position_loaded(Vector3 position);

	Chunk* get_chunk_at(Vector3i position);

	// Boilerplate setters and getters
	void set_instance_radius(int64_t new_radius);
	int64_t get_instance_radius() const;
	TypedArray<Block> get_block_types() const;
    void set_block_types(const TypedArray<Block> new_block_types);
	Ref<Material> get_block_material() const;
    void set_block_material(Ref<Material> new_material);
	Ref<NoiseTexture2D> get_main_noise_texture() const;
    void set_main_noise_texture(Ref<NoiseTexture2D> new_texture);
};

}