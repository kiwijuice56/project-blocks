
#pragma once

#include "../include/block.h"
#include "../include/generator.h"
#include "../include/chunk.h"
#include "../include/decoration.h"
#include "../include/item_state.h"

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

protected:
	static void _bind_methods();

public:
	static const int64_t MAX_DECORATIONS = 16;
	static const int64_t MAX_WATER_SUBCHUNKS_PER_FRAME = 128;
	static const int64_t MAX_WATER_RERENDERED_CHUNKS_PER_FRAME = 4;

	// (In blocks)
	int64_t instance_radius = 128;
	int64_t water_simulate_radius = 64;

	// Resources
	TypedArray<Block> block_types;
	TypedArray<Decoration> decorations;
	Ref<ShaderMaterial> block_material;
	Ref<ShaderMaterial> transparent_block_material;
	Ref<ShaderMaterial> water_material;
	Ref<ShaderMaterial> water_ghost_material;
	Ref<ShaderMaterial> water_surface_material;
	Ref<PackedScene> dropped_item_scene; // These (and below) are set in code for convenience
	Ref<PackedScene> break_effect_scene;
	Ref<PackedScene> place_effect_scene;
	Ref<Generator> generator;

	// The center chunk's position
	Vector3i center_chunk;

	// The center position
	Vector3i center;

	// Stores references to all chunk node instances
	std::vector<Chunk*> all_chunks;

	// Stores coordinates (Vector3i) of loaded chunks
	Dictionary is_chunk_loaded;

	// Stores (Vector3i : Chunk) mapping for easy access
	Dictionary chunk_map;

	// Stores (Vector3i : bool) for which chunks have decorations generated
	Dictionary decoration_generated;

	// Stores data of modified chunks (Vector3i : PackedInt32Array)
	Dictionary chunk_data;
	Dictionary chunk_water_data; // PackedByteArray
	Dictionary chunk_water_awake_data; // PackedByteArray

	// Used to access chunks that need to be initialized
	std::vector<Chunk*> init_queue;
	std::vector<Vector3i> init_queue_positions;

	// Multithreading state
	uint64_t task_id = 0;
	bool has_task = false;
	std::mutex decoration_lock;

	// Water state
	uint8_t water_frame = 0;
	uint16_t simulated_water_subchunks = 0;
	uint16_t rendered_water_chunks = 0;

	// Stores (Vector3i : Array[Decoration])
	Dictionary decoration_map;

	// Stores (Vector3i : int) [size of above arrays]
	Dictionary decoration_count;

	// Stores (int : int)
	Dictionary block_id_to_index_map;

	// Stores (String : Resource) name to Decoration pairs
	Dictionary decoration_name_map;

	// Stores (String : int) name to index pairs
	Dictionary block_name_map;

	World();
	~World();

	void initialize();
	void create_texture_atlas();
	void instantiate_chunks();

	// Used to set the loaded region of the world, new_center is usually the player's position
	void set_loaded_region_center(Vector3 new_center);
	void update_loaded_region();
	void initialize_chunk(uint64_t index); // MT
	void initialize_chunk_decorations(uint64_t index); // MT

	// Helpful interfacing methods
	void simulate_water();
	bool is_position_loaded(Vector3 position);
	bool is_chunk_in_radius(Vector3i coordinate, int64_t radius);
	Chunk* get_chunk_at(Vector3 position); // Gets the chunk nearest to the given position
	Vector3i snap_to_chunk(Vector3 position); // Snaps a position to the nearest chunk's position
	Ref<Block> get_block_type_at(Vector3 position);
	void break_block_at(Vector3 position, bool drop_item, bool play_effect);
	void place_block_at(Vector3 position, Ref<Block> block_type, bool play_effect);
	void place_decoration(Ref<Decoration> decoration, Vector3i position);
	void place_water_at(Vector3 position, uint8_t amount);
	uint8_t get_water_level(Vector3 position);
	bool is_under_water(Vector3 position);

	// Boilerplate setters and getters
	void set_instance_radius(int64_t new_radius);
	int64_t get_instance_radius() const;

	TypedArray<Decoration> get_decorations() const;
	void set_decorations(TypedArray<Decoration> new_decorations);

	TypedArray<Block> get_block_types() const;
    void set_block_types(const TypedArray<Block> new_block_types);

	Ref<ShaderMaterial> get_block_material() const;
    void set_block_material(Ref<ShaderMaterial> new_material);

	Ref<ShaderMaterial> get_transparent_block_material() const;
    void set_transparent_block_material(Ref<ShaderMaterial> new_material);

	Ref<ShaderMaterial> get_water_material() const;
    void set_water_material(Ref<ShaderMaterial> new_material);

	Ref<ShaderMaterial> get_water_ghost_material() const;
    void set_water_ghost_material(Ref<ShaderMaterial> new_material);

	Ref<ShaderMaterial> get_water_surface_material() const;
    void set_water_surface_material(Ref<ShaderMaterial> new_material);

	Ref<Generator> get_generator() const;
    void set_generator(Ref<Generator> new_generator);
};

}