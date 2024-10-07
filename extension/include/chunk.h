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
    Ref<Material> block_material;
    Ref<NoiseTexture2D> main_noise_texture;
    ConcavePolygonShape3D* shape_data;

    // Block ID data
    PackedByteArray blocks;

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
    static const int64_t CHUNK_SIZE_X = 32;
    static const int64_t CHUNK_SIZE_Y = 32;
    static const int64_t CHUNK_SIZE_Z = 32;

    bool completely_filled = false;

	Chunk();
	~Chunk();

    // Boilerplate setters and getters
    Ref<Material> get_block_material() const;
    void set_block_material(Ref<Material> new_material);
    Ref<NoiseTexture2D> get_main_noise_texture() const;
    void set_main_noise_texture(Ref<NoiseTexture2D> new_texture);

    // Generation methods
    void generate_mesh();
    void generate_data(Vector3i chunk_position);
    void clear_collision();

    // Helper methods to interface with internal data structures
    uint64_t get_block_id_at(Vector3i position);
    uint64_t position_to_index(Vector3i position);
    Vector3i index_to_position(uint64_t index);
    double sample_from_noise(Ref<NoiseTexture2D> texture, Vector2 uv);
    bool in_bounds(Vector3i position);

    // Modifying methods
    void remove_block_at(Vector3i global_position);
};

}

