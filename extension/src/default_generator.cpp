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

void DefaultGenerator::generate_terrain_blocks(World* world, Chunk* chunk, Array decorations, Vector3i chunk_position) {
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
                    block_type = world->block_name_map["grass"];
                } else if (0 < block_height - real_height && block_height - real_height < 5 ) {
                    block_type = world->block_name_map["dirt"];
                } else if (real_height < block_height) {
                    block_type = world->block_name_map["stone"];
                }

                chunk->blocks[Chunk::position_to_index(Vector3i(x, y, z))] = block_type;
            }
        }
    }

    generate_decoration_blocks(world, chunk, decorations, chunk_position);
}

void DefaultGenerator::generate_decorations(World* world, Vector3i chunk_position) {
    Ref<Decoration> tree = Object::cast_to<Decoration>(world->decoration_name_map["tree"]);
    world->place_decoration(tree, chunk_position + Vector3i(0, 0, -1));
}

Ref<NoiseTexture2D> DefaultGenerator::get_main_noise_texture() const {
    return main_noise_texture;
}

void DefaultGenerator::set_main_noise_texture(Ref<NoiseTexture2D> new_texture) {
    main_noise_texture = new_texture;
}