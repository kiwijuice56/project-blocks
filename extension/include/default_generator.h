
#pragma once

#include "../include/generator.h"

namespace godot {

class DefaultGenerator : public Generator {
	GDCLASS(DefaultGenerator, Generator)

private:
	Ref<NoiseTexture2D> main_noise_texture;

protected:
	static void _bind_methods();

public:
	DefaultGenerator();
	~DefaultGenerator();

	void generate_decorations(World* world, Vector3i chunk_position);
	void generate_blocks(Chunk* chunk, Array decorations, Vector3i chunk_position);

	Ref<NoiseTexture2D> get_main_noise_texture() const;
    void set_main_noise_texture(Ref<NoiseTexture2D> new_texture);
};
}