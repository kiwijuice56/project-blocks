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
    Ref<Material> block_material;
    Ref<NoiseTexture2D> main_noise_texture;

    PackedByteArray blocks;
    PackedVector3Array vertices;
    PackedInt32Array indices;
    PackedVector2Array uvs;
    PackedVector3Array normals;
    PackedVector3Array collision_vertices;

    uint64_t id = 0;
    uint64_t face_count = 0;
    uint64_t block_count = 0;
    // Easy optimization: skip checking blocks above the maximum non-empty Y,
    // as most chunks have huge spaces of empty air over them
    uint64_t max_y = 0;
    double texture_scale = 1/8.0;
    bool has_static_body = false;

protected:
	static void _bind_methods();

    // Helper methods to generate chunk mesh
    void add_face_uvs(uint64_t x, uint64_t y);
    void add_face_normals(Vector3 normal);
    void add_face_triangles();
    void generate_block_faces(uint64_t id, Vector3i position);

public:
    // The dimensions of individual chunks
    static const int64_t CHUNK_SIZE_X = 8;
    static const int64_t CHUNK_SIZE_Y = 128;
    static const int64_t CHUNK_SIZE_Z = 8;

	Chunk();
	~Chunk();

    uint64_t get_id() const;
	void set_id(uint64_t new_id);

    Ref<Material> get_block_material() const;
    void set_block_material(Ref<Material> new_material);

    Ref<NoiseTexture2D> get_main_noise_texture() const;
    void set_main_noise_texture(Ref<NoiseTexture2D> new_texture);

    void generate_mesh();
    void generate_data();
    void generate_static_body(bool force_update);

    // Helper methods to access data
    uint64_t get_block_id_at(Vector3 position);
    uint64_t position_to_index(Vector3 position);
    Vector3 index_to_position(uint64_t index);
    double sample_from_noise(Ref<NoiseTexture2D> texture, Vector2 uv);
    bool in_bounds(Vector3 position);

    // Modifying methods

    void remove_block_at(Vector3 global_position);
};

}

