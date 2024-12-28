
#pragma once

#include "../include/generator.h"

namespace godot {

class DebugGenerator : public Generator {
	GDCLASS(DebugGenerator, Generator)

private:

protected:
	static void _bind_methods();

public:
	DebugGenerator();
	~DebugGenerator();

	void generate_decorations(World* world, Vector3i chunk_position);
	void generate_blocks(World* world, Chunk* chunk, Array decorations, Vector3i chunk_position);
};
}