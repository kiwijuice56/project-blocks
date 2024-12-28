
#pragma once

#include <godot_cpp/classes/noise_texture2D.hpp>
#include <godot_cpp/classes/image.hpp>

namespace godot {

class Chunk;
class World;

class Generator : public Resource {
	GDCLASS(Generator, Resource)

private:
	Ref<NoiseTexture2D> main_noise_texture;

protected:
	static void _bind_methods();

public:
	Generator();
	~Generator();

	// Given a chunk's position, generate any decorations within that chunk
	// (using World::place_decoration)
	virtual void generate_decorations(World* world, Vector3i chunk_position);

	// Given a chunk's position and decorations, fill in the terrain blocks
	// This function should not fill in a decoration's blocks
	virtual void generate_terrain_blocks(World* world, Chunk* chunk, Array decorations, Vector3i chunk_position);

	// Place down blocks of decorations that are in range of this chunk
	void generate_decoration_blocks(World* world, Chunk* chunk, Array decorations, Vector3i chunk_position);

	double sample_from_noise(Ref<NoiseTexture2D> texture, Vector2 uv);
};
}