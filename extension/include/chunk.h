#pragma once

#include <godot_cpp/classes/mesh_instance3d.hpp>

namespace godot {

class Chunk : public MeshInstance3D {
	GDCLASS(Chunk, MeshInstance3D)

private:
    PackedVector3Array vertices;
    PackedInt32Array indices;
    PackedVector2Array uvs;

    int face_count = 0;
    float texture_scale = 0.125;

protected:
	static void _bind_methods();

    void generate_cube(Vector3 position);
    void add_face_uvs(int x, int y);
    void add_face_triangles();
    void generate_mesh();

public:
	Chunk();
	~Chunk();
};

}

