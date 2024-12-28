#include "../include/default_generator.h"
#include "../include/chunk.h"
#include "../include/world.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void DefaultGenerator::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_main_noise_texture"), &DefaultGenerator::get_main_noise_texture);
	ClassDB::bind_method(D_METHOD("set_main_noise_texture", "new_texture"), &DefaultGenerator::set_main_noise_texture);

    ADD_PROPERTY(
        PropertyInfo(
            Variant::OBJECT,
            "main_noise_texture",
            PROPERTY_HINT_RESOURCE_TYPE,
            "NoiseTexture2D"
        ),
        "set_main_noise_texture",
        "get_main_noise_texture"
    );
}

DefaultGenerator::DefaultGenerator() { }

DefaultGenerator::~DefaultGenerator() { }


void DefaultGenerator::generate_blocks(World* world, Chunk* chunk, Array decorations, Vector3i chunk_position) {
    Vector2 chunk_uv = Vector2(
        Vector2i(chunk_position.x, chunk_position.z)
        / Vector2i(Chunk::CHUNK_SIZE_X, Chunk::CHUNK_SIZE_Z)
        % Vector2i(32, 32)
    ) / 32.0;

    if (chunk_uv.x < 0.0) chunk_uv.x = 1.0 + chunk_uv.x;
    if (chunk_uv.y < 0.0) chunk_uv.y = 1.0 + chunk_uv.y;

    for (int64_t z = 0; z < Chunk::CHUNK_SIZE_Z; z++) {
        for (int64_t x = 0; x < Chunk::CHUNK_SIZE_X; x++) {
            Vector2 uv = chunk_uv + Vector2(x, z) / Vector2(Chunk::CHUNK_SIZE_X, Chunk::CHUNK_SIZE_Z) / 32.0;

            double height = sample_from_noise(main_noise_texture, uv);
            int64_t block_height = 1 + int(height * 64);
            for (int64_t y = 0; y < Chunk::CHUNK_SIZE_Y; y++) {
                int64_t real_height = y + chunk_position.y;
                uint64_t block_type = 0;

                if (block_height == real_height) {
                    block_type = 128455936;
                } else if (0 < block_height - real_height && block_height - real_height < 5 ) {
                    block_type = 324041069;
                } else if (real_height < block_height) {
                    block_type = 210957605;
                }

                chunk->blocks[Chunk::position_to_index(Vector3i(x, y, z))] = world->block_id_to_index_map[block_type];
            }
        }
    }

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

void DefaultGenerator::generate_decorations(World* world, Vector3i chunk_position) {
    Ref<Decoration> d = Object::cast_to<Decoration>(world->decorations["heap"]);
    world->place_decoration(d, chunk_position + Vector3i(0, 0, -1));
}

Ref<NoiseTexture2D> DefaultGenerator::get_main_noise_texture() const {
    return main_noise_texture;
}

void DefaultGenerator::set_main_noise_texture(Ref<NoiseTexture2D> new_texture) {
    main_noise_texture = new_texture;
}