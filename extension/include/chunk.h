#pragma once

#include "../include/block.h"
#include "../include/decoration.h"

#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/material.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/classes/concave_polygon_shape3d.hpp>
#include <godot_cpp/classes/static_body3d.hpp>
#include <godot_cpp/classes/collision_shape3d.hpp>
#include <godot_cpp/classes/box_mesh.hpp>
#include <godot_cpp/classes/resource_loader.hpp>

namespace godot {

class World;

class Chunk : public MeshInstance3D {
	GDCLASS(Chunk, MeshInstance3D)

protected:
    static void _bind_methods();

public:
    // The dimensions of individual chunks
    static const int64_t CHUNK_SIZE_X = 16;
    static const int64_t CHUNK_SIZE_Y = 16;
    static const int64_t CHUNK_SIZE_Z = 16;

    // The dimensions of individual chunks
    static const int64_t WATER_CHUNK_SIZE_X = 4;
    static const int64_t WATER_CHUNK_SIZE_Y = 4;
    static const int64_t WATER_CHUNK_SIZE_Z = 4;

    // Block + water index data
    PackedInt32Array blocks;
    PackedByteArray water; // Stores numerical density 0-255
    PackedByteArray water_buffer; // Buffer between simulation steps
    PackedByteArray water_chunk_awake; // Stores whether each water subchunk should be awake

    // Mesh data
    PackedVector3Array vertices;
    PackedVector3Array normals;
    PackedVector2Array uvs;
    PackedVector2Array uvs2;

    // Resources
    ConcavePolygonShape3D* shape_data_opaque;
    ConcavePolygonShape3D* shape_data_transparent;
    MeshInstance3D* water_mesh;

    // Resources set by World
    World* world;
    TypedArray<Block> block_types;
    Ref<ShaderMaterial> block_material;
	Ref<ShaderMaterial> transparent_block_material;

    // Water debugging tool
    TypedArray<MeshInstance3D> water_indicators;

    // Greedy meshing state variables
    bool* visited;
    uint64_t current_greedy_block = 0;

    // Calculated state variables
    uint64_t face_count = 0; // Quads, not tris
    uint64_t block_count = 0;
    uint64_t water_count = 0;
    bool uniform = false; // Used to optimize chunks of one block type
    bool modified = false; // Whether this chunk has had any blocks placed/removed
    bool never_initialized = true;
    bool water_updated = false;

    Chunk();
	~Chunk();

    // External interfacing methods
    void remove_block_at(Vector3i global_position);
    void place_block_at(Vector3i global_position, uint32_t block_index);
    uint64_t get_block_index_at_global(Vector3i global_position);
    void set_water_at_global(Vector3i global_position);
    void get_water_at_global(Vector3i global_position);

    // Internal interfacing methods
    uint64_t get_block_index_at(Vector3i position);
    static uint64_t position_to_index(Vector3i position);
    Vector3i index_to_position(uint64_t index);
    static bool in_bounds(Vector3i position);

    // Mesh generation methods
    void generate_mesh(bool immediate);
    void generate_water_mesh();
    void calculate_block_statistics();
    void clear_collision();

    // Helper methods to generate chunk mesh
    void add_face_uvs(uint64_t id, Vector2i scale);
    void add_face_normals(Vector3i normal);
    void add_rectangular_prism(Vector3 start, Vector3 size);
    void greedy_mesh_generation(bool transparent, bool water_pass);
    Vector3i greedy_scan(Vector3i start, bool water_pass);
    bool greedy_invalid(Vector3i position, bool water_pass);

    // Water simulation
    uint8_t get_water_at(Vector3i local_position);
    void set_water_at(Vector3i local_position, uint8_t water);
    void simulate_water();
    void water_chunk_wake_set(Vector3i local_position, bool awake, bool surround);
    bool is_valid_water(Vector3i local_position);
};
}

