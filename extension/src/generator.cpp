#include "../include/chunk.h"
#include "../include/world.h"
#include "../include/generator.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void Generator::_bind_methods() {

}

Generator::Generator() { }

Generator::~Generator() { }

void Generator::generate_terrain_blocks(World* world, Chunk* chunk, Array decorations, Vector3i chunk_position) {
    chunk->blocks.fill(0);
}

void Generator::generate_decorations(World* world, Vector3i chunk_position) {

}

void Generator::generate_decoration_blocks(World* world, Chunk* chunk, Array decorations, Vector3i chunk_position) {
    for (int64_t i = 0; i < decorations.size(); i++) {
        Ref<Decoration> d = Object::cast_to<Decoration>(decorations[i]);
        for (int64_t y = 0; y < d->get_size().y; y++) {
            for (int64_t z = 0; z < d->get_size().z; z++) {
                for (int64_t x = 0; x < d->get_size().x; x++) {
                    Vector3i local_position = Vector3i(x, y, z) + d->position - chunk_position;
                    if (!Chunk::in_bounds(local_position)) continue;
                    int32_t block_id = d->get_blocks()[x + z * d->get_size().x + y * d->get_size().x * d->get_size().z];

                    // Don't override with air blocks
                    if (block_id == 0) continue;

                    // But replace void blocks with air (that does override)
                    if (block_id == 1) block_id = 0;

                    int32_t index = world->block_id_to_index_map[block_id];
                    chunk->blocks[Chunk::position_to_index(local_position)] = index;
                }
            }
        }
    }
}

double Generator::sample_from_noise(Ref<NoiseTexture2D> texture, Vector2 uv) {
    Ref<Image> img = texture->get_image();
    return img->get_pixel(uint64_t(uv.x * (img->get_width() - 1)), uint64_t(uv.y * (img->get_height() - 1))).r;
}