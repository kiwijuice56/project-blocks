#pragma once

#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/material.hpp>
#include <godot_cpp/classes/noise_texture2d.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/concave_polygon_shape3d.hpp>
#include <godot_cpp/classes/static_body3d.hpp>
#include <godot_cpp/classes/collision_shape3d.hpp>

namespace godot {
class Chunk : public MeshInstance3D {
	GDCLASS(Chunk, MeshInstance3D)

private:
    // Resources
    ConcavePolygonShape3D* shape_data;

    // Greedy meshing state variables
    bool* visited;
    uint64_t current_greedy_block = 0;

    // Mesh data
    PackedVector3Array vertices;
    PackedVector3Array normals;
    PackedVector2Array uvs;
    PackedVector2Array uvs2;

    uint64_t face_count = 0; // Quads, not tris
    uint64_t block_count = 0;

    bool uniform = false; // Used to optimize chunks of one block type

protected:
	static void _bind_methods();

    // Helper methods to generate chunk mesh
    void add_face_uvs(uint64_t id, Vector2i scale);
    void add_face_normals(Vector3i normal);
    void add_rectangular_prism(Vector3i start, Vector3i size);
    void greedy_mesh_generation();
    Vector3i greedy_scan(Vector3i start);
    bool greedy_invalid(Vector3i position);

public:
    // The dimensions of individual chunks
    static const int64_t CHUNK_SIZE_X = 16;
    static const int64_t CHUNK_SIZE_Y = 16;
    static const int64_t CHUNK_SIZE_Z = 16;

    // Block ID data
    PackedInt32Array blocks;

    bool modified = false; // Whether this chunk has had any blocks placed/removed
    bool never_initialized = true;

    // Resources set by World
    Ref<NoiseTexture2D> main_noise_texture;

    Chunk();
	~Chunk();

    // Generation methods
    void generate_mesh(bool immediate);
    void generate_data(Vector3i chunk_position, bool override);
    void clear_collision();

    // Helper methods to interface with internal data structures
    uint64_t get_block_id_at(Vector3i position);
    uint64_t position_to_index(Vector3i position);
    Vector3i index_to_position(uint64_t index);
    double sample_from_noise(Ref<NoiseTexture2D> texture, Vector2 uv);
    bool in_bounds(Vector3i position);

    // Modifying methods
    void remove_block_at(Vector3i global_position);
    void place_block_at(Vector3i global_position, uint8_t block_id);
};
}

