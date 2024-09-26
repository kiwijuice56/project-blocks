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

    int face_count = 0;
    float texture_scale = 0.125;

protected:
	static void _bind_methods();



    void add_face_uvs(int64_t x, int64_t y);
    void add_face_triangles();

    void generate_cube(Vector3 position);
    void generate_chunk();
    void generate_mesh();

public:
    const int CHUNK_SIZE_X = 16;
    const int CHUNK_SIZE_Y = 128;
    const int CHUNK_SIZE_Z = 16;

	Chunk();
	~Chunk();

    int64_t get_block_id_at(Vector3 position);
};

}

