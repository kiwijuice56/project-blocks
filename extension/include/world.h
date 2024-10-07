
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

	TypedArray<Block> block_types;
	Ref<Material> block_material;
	Ref<NoiseTexture2D> main_noise_texture;

	Vector3 center;
	Vector3i center_chunk;

	std::vector<Chunk*> all_chunks;

	// Used to access chunks that need to be initialized (multithreaded)
	std::vector<Chunk*> initiliazation_queue;
	std::vector<Vector3> initiliazation_queue_positions;

	uint64_t task_id = 0;
	bool has_task = false;
	bool is_task_data = false;

protected:
	static void _bind_methods();

	void regenerate_chunks();
	void initialize_chunk_data(uint64_t index);
	void initialize_chunk_mesh(uint64_t index);
	void update_loaded_region();

	bool is_chunk_in_radius(Vector3i coordinate, int64_t radius);

public:
	World();
	~World();

	void initialize();

	// Boilerplate setters and getters
	void set_instance_radius(int64_t new_radius);
	int64_t get_instance_radius() const;
	TypedArray<Block> get_block_types() const;
    void set_block_types(const TypedArray<Block> new_block_types);
	Ref<Material> get_block_material() const;
    void set_block_material(Ref<Material> new_material);
	Ref<NoiseTexture2D> get_main_noise_texture() const;
    void set_main_noise_texture(Ref<NoiseTexture2D> new_texture);
	Vector3 get_center() const;
	void set_center(Vector3 new_center);
};

}