
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

	void generate_decorations(World* world, Vector3i chunk_position);
	void generate_blocks(Chunk* chunk, Array decorations, Vector3i chunk_position);

	double sample_from_noise(Ref<NoiseTexture2D> texture, Vector2 uv);

	Ref<NoiseTexture2D> get_main_noise_texture() const;
    void set_main_noise_texture(Ref<NoiseTexture2D> new_texture);
};
}