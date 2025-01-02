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
}

void DebugGenerator::generate_decorations(World* world, Vector3i chunk_position) {

}
