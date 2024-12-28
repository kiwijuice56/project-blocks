#include "../include/chunk.h"
#include "../include/world.h"
#include "../include/generator.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void Generator::_bind_methods() {

}

Generator::Generator() { }

Generator::~Generator() { }

void Generator::generate_blocks(World* world, Chunk* chunk, Array decorations, Vector3i chunk_position) {
    chunk->blocks.fill(0);
}

void Generator::generate_decorations(World* world, Vector3i chunk_position) {

}

double Generator::sample_from_noise(Ref<NoiseTexture2D> texture, Vector2 uv) {
    Ref<Image> img = texture->get_image();
    return img->get_pixel(uint64_t(uv.x * (img->get_width() - 1)), uint64_t(uv.y * (img->get_height() - 1))).r;
}