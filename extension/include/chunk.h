#pragma once

#include <godot_cpp/classes/mesh_instance3d.hpp>

namespace godot {

class Chunk : public MeshInstance3D {
	GDCLASS(Chunk, MeshInstance3D)

private:

protected:
	static void _bind_methods();

public:
	Chunk();
	~Chunk();
};

}

