
#pragma once

#include "../include/block.h"
#include "../include/chunk.h"
#include "../include/decoration.h"

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
	Ref<ShaderMaterial> transparent_block_material;
	Ref<NoiseTexture2D> main_noise_texture;
	Dictionary decorations;

	Ref<PackedScene> dropped_item_scene;
	Ref<PackedScene> break_effect_scene;

	// The center chunk's position
	Vector3i center_chunk;

	// Stores references to all chunk node instances
	std::vector<Chunk*> all_chunks;

	// Stores coordinates (Vector3i) of loaded chunks
	Dictionary is_chunk_loaded;

	// Stores (Vector3i : Chunk) mapping for easy access
	Dictionary chunk_map;

	// Stores (Vector3i : Array[Decoration])
	Dictionary decoration_map;

	// Stores block data of modified chunks (Vector3i : PackedInt32Array)
	Dictionary chunk_data;

	// Used to access chunks that need to be initialized
	std::vector<Chunk*> init_queue;
	std::vector<Vector3i> init_queue_positions;

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

	// Helpful interfacing methods
	bool is_position_loaded(Vector3 position);
	Chunk* get_chunk_at(Vector3 position); // Gets the chunk nearest to the given position
	Vector3i snap_to_chunk(Vector3 position); // Snaps a position to the nearest chunk's position
	Ref<Block> get_block_type_at(Vector3 position);
	void break_block_at(Vector3 position, bool drop_item, bool play_effect);
	void place_block_at(Vector3 position, uint8_t block_type);

	// Boilerplate setters and getters
	void set_instance_radius(int64_t new_radius);
	int64_t get_instance_radius() const;

	Dictionary get_decorations() const;
	void set_decorations(Dictionary new_decorations);

	TypedArray<Block> get_block_types() const;
    void set_block_types(const TypedArray<Block> new_block_types);

	Ref<ShaderMaterial> get_block_material() const;
    void set_block_material(Ref<ShaderMaterial> new_material);

	Ref<ShaderMaterial> get_transparent_block_material() const;
    void set_transparent_block_material(Ref<ShaderMaterial> new_material);

	Ref<NoiseTexture2D> get_main_noise_texture() const;
    void set_main_noise_texture(Ref<NoiseTexture2D> new_texture);
};

}