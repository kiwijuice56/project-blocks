#pragma once

#include <godot_cpp/classes/mesh_instance3d.hpp>

namespace godot {
class Chunk : public MeshInstance3D {
	GDCLASS(Chunk, MeshInstance3D)

private:
    PackedInt64Array blocks;
    PackedVector3Array vertices;
    PackedInt32Array indices;
    PackedVector2Array uvs;

    int64_t face_count = 0;
    double texture_scale = 1/8.0;

protected:
	static void _bind_methods();

    void add_face_uvs(uint64_t x, uint64_t y);
    void add_face_triangles();

    void generate_block(uint64_t id, Vector3i position);
    void generate_chunk();
    void generate_mesh();

public:
    const uint64_t CHUNK_SIZE_X = 16;
    const uint64_t CHUNK_SIZE_Y = 128;
    const uint64_t CHUNK_SIZE_Z = 16;

	Chunk();
	~Chunk();

    // Helper methods
    uint64_t get_block_id_at(Vector3 position);
    uint64_t position_to_index(Vector3 position);
    Vector3 index_to_position(uint64_t index);
    bool in_bounds(Vector3 position);
};

}

