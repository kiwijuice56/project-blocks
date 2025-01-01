#include "../include/debug_generator.h"
#include "../include/chunk.h"
#include "../include/world.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void DebugGenerator::_bind_methods() {

}

DebugGenerator::DebugGenerator() { }

DebugGenerator::~DebugGenerator() { }

void DebugGenerator::generate_terrain_blocks(World* world, Chunk* chunk, Array decorations, Vector3i chunk_position) {
    chunk->blocks.fill(0);

    if (chunk_position.length() == 0) {
        chunk->blocks.fill(2);
    }
}

void DebugGenerator::generate_water(World* world, Chunk* chunk, Array decorations, Vector3i chunk_position) {
    chunk->water.fill(0);
    if (chunk_position == Vector3i(0, 16, 0)) {
        chunk->has_water = true;
        for (uint64_t x = 0; x < Chunk::CHUNK_SIZE_X; x++) {
            for (uint64_t z = 0; z < Chunk::CHUNK_SIZE_Z; z++) {
                for (uint64_t y = 0; y < Chunk::CHUNK_SIZE_Y; y++) {
                    if (y >= 8 && y <= 12 && x >= 6 && x <= 10 && z >= 6 && z <= 10) {
                        chunk->water[Chunk::position_to_index(Vector3i(x, y, z))] = 255;
                    }
                }
            }
        }
    }
}

void DebugGenerator::generate_decorations(World* world, Vector3i chunk_position) {

}
