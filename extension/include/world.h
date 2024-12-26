
#pragma once

#include "../include/block.h"
#include "../include/chunk.h"

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/texture2d_array.hpp>
#include <godot_cpp/classes/worker_thread_pool.hpp>
#include <godot_cpp/classes/thread.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/resource_loader.hpp>

namespace godot {

class World : public Node3D {
	GDCLASS(World, Node3D)

private:
	int64_t instance_radius = 128;

	// Resources
	TypedArray<Block> block_types;
	Ref<ShaderMaterial> block_material;
	Ref<NoiseTexture2D> main_noise_texture;
	Ref<PackedScene> dropped_item_scene;

	// The center chunk's position
	Vector3i center_chunk;

	// Stores references to all chunk node instances
	std::vector<Chunk*> all_chunks;

	// Stores coordinates (Vector3i) of loaded chunks
	Dictionary is_chunk_loaded;

	// Stores (Vector3i : Chunk) mapping for easy access
	Dictionary chunk_map;

	// Stores block data of modified chunks (Vector3i : PackedInt32Array)
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
	void create_texture_atlas();

	bool is_chunk_in_radius(Vector3i coordinate, int64_t radius);

public:
	World();
	~World();

	void initialize();

	// Used to set the loaded region of the world, new_center is usually the player's position
	void set_loaded_region_center(Vector3 new_center);

	// Other helpful interfacing methods
	bool is_position_loaded(Vector3 position);
	Chunk* get_chunk_at(Vector3i position);
	Vector3i snap_to_chunk(Vector3 position);

	Block* get_block_type_at(Vector3i position);
	void break_block_at(Vector3i position, bool drop_item);
	void place_block_at(Vector3i position, uint8_t block_type);

	// Boilerplate setters and getters
	void set_instance_radius(int64_t new_radius);
	int64_t get_instance_radius() const;

	TypedArray<Block> get_block_types() const;
    void set_block_types(const TypedArray<Block> new_block_types);

	Ref<ShaderMaterial> get_block_material() const;
    void set_block_material(Ref<ShaderMaterial> new_material);

	Ref<NoiseTexture2D> get_main_noise_texture() const;
    void set_main_noise_texture(Ref<NoiseTexture2D> new_texture);

};

}