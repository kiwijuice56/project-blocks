#include "../include/world.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void World::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_block_types"), &World::get_block_types);
	ClassDB::bind_method(D_METHOD("set_block_types", "new_block_types"), &World::set_block_types);

    ClassDB::bind_method(D_METHOD("get_center"), &World::get_center);
	ClassDB::bind_method(D_METHOD("set_center", "new_center"), &World::set_center);

    ClassDB::add_property(
        "World",
        PropertyInfo(
            Variant::ARRAY,
            "block_types",
            PROPERTY_HINT_TYPE_STRING,
            String::num(Variant::OBJECT) + "/" + String::num(PROPERTY_HINT_RESOURCE_TYPE) + ":Block"
        ),
        "set_block_types",
        "get_block_types"
    );

    ClassDB::add_property(
        "World",
        PropertyInfo(Variant::VECTOR3, "center"),
        "set_center",
        "get_center"
    );
}

World::World() {
    stored_chunks = Dictionary();
}

World::~World() {

}

void World::set_block_types(TypedArray<Block> new_block_types) {
    block_types = new_block_types;
}

TypedArray<Block> World::get_block_types() const {
    return block_types;
}

Vector3 World::get_center() const {
    return center;
}

void World::set_center(Vector3 new_center) {
    Vector3i new_center_chunk = Vector3i(new_center / Vector3(Chunk::CHUNK_SIZE_X, 0, Chunk::CHUNK_SIZE_Z));
    if (new_center_chunk != center_chunk) {
        center_chunk = new_center_chunk;
        update_loaded_region();
    }

    center = new_center;
}

void World::generate_chunk(Vector3i coordinate) {
    if (stored_chunks.has(coordinate)) {
        add_child(Object::cast_to<Node>(stored_chunks[coordinate]));
    } else {
        Chunk* new_chunk = memnew(Chunk);
        new_chunk->generate_chunk();
        new_chunk->generate_mesh();

        add_child(new_chunk);

        new_chunk->set_position(coordinate * Vector3(Chunk::CHUNK_SIZE_X, 0, Chunk::CHUNK_SIZE_Z));

        stored_chunks[coordinate] = new_chunk;
    }
}

void World::update_loaded_region() {
    Dictionary loaded = Dictionary();

    for (uint64_t i = 0; i < get_child_count(); i++) {
        Chunk* chunk = Object::cast_to<Chunk>(get_child(i));
        Vector3i chunk_coordinate = Vector3i(chunk->get_position()) / Vector3i(Chunk::CHUNK_SIZE_X, 1, Chunk::CHUNK_SIZE_Z);
        UtilityFunctions::print(chunk_coordinate);
        if ((chunk_coordinate - center_chunk).length_squared() > load_radius * load_radius) {
            remove_child(chunk);
        } else {
            loaded[chunk_coordinate] = true;
        }
    }

    UtilityFunctions::print("!!!");

    for (int64_t chunk_x = -load_radius; chunk_x <= load_radius; chunk_x++) {
        for (int64_t chunk_z = -load_radius; chunk_z <= load_radius; chunk_z++) {
            if (chunk_x * chunk_x + chunk_z * chunk_z > load_radius * load_radius) {
                continue;
            }
            UtilityFunctions::print(Vector3i(center_chunk.x + chunk_x, 0, center_chunk.z + chunk_z));
            if (loaded.has(Vector3i(center_chunk.x + chunk_x, 0, center_chunk.z + chunk_z))) {
                continue;
            }

            generate_chunk(Vector3i(center_chunk.x + chunk_x, 0, center_chunk.z + chunk_z));
        }
    }
}
