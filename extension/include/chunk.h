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
    PackedVector3Array normals;

    uint64_t id = 0;
    uint64_t face_count = 0;
    double texture_scale = 1/8.0;

protected:
	static void _bind_methods();

    void add_face_uvs(uint64_t x, uint64_t y);
    void add_face_triangles();
    void generate_block_faces(uint64_t id, Vector3i position);

public:
    static const uint64_t CHUNK_SIZE_X = 16;
    static const uint64_t CHUNK_SIZE_Y = 128;
    static const uint64_t CHUNK_SIZE_Z = 16;

	Chunk();
	~Chunk();

    uint64_t get_id() const;
	void set_id(uint64_t new_id);

    void generate_mesh();
    void generate_chunk();

    // Helper methods
    uint64_t get_block_id_at(Vector3 position);
    uint64_t position_to_index(Vector3 position);
    Vector3 index_to_position(uint64_t index);
    bool in_bounds(Vector3 position);
};

}

