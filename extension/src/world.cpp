#include "../include/world.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void World::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_block_types"), &World::get_block_types);
	ClassDB::bind_method(D_METHOD("set_block_types", "new_block_types"), &World::set_block_types);

    ClassDB::bind_method(D_METHOD("get_center"), &World::get_center);
	ClassDB::bind_method(D_METHOD("set_center", "new_center"), &World::set_center);

    ClassDB::bind_method(D_METHOD("get_block_material"), &World::get_block_material);
	ClassDB::bind_method(D_METHOD("set_block_material", "new_material"), &World::set_block_material);

    ClassDB::bind_method(D_METHOD("get_main_noise_texture"), &World::get_main_noise_texture);
	ClassDB::bind_method(D_METHOD("set_main_noise_texture", "new_texture"), &World::set_main_noise_texture);

    ClassDB::bind_method(D_METHOD("generate_from_queue", "n"), &World::generate_from_queue);

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

    ClassDB::add_property(
        "World",
        PropertyInfo(
            Variant::OBJECT,
            "block_material",
            PROPERTY_HINT_RESOURCE_TYPE,
            "Material"
        ),
        "set_block_material",
        "get_block_material"
    );

    ClassDB::add_property(
        "World",
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

World::World() {
    loaded_chunks = Dictionary();
    is_chunk_in_queue = Dictionary();
    initiliazation_queue = TypedArray<Chunk>();
}

World::~World() {

}

void World::set_block_types(TypedArray<Block> new_block_types) {
    block_types = new_block_types;
}

TypedArray<Block> World::get_block_types() const {
    return block_types;
}

Ref<Material> World::get_block_material() const {
    return block_material;
}

void World::set_block_material(Ref<Material> new_material) {
    block_material = new_material;
}

Ref<NoiseTexture2D> World::get_main_noise_texture() const {
    return main_noise_texture;
}

void World::set_main_noise_texture(Ref<NoiseTexture2D> new_texture) {
    main_noise_texture = new_texture;
}

Vector3 World::get_center() const {
    return center;
}

void World::set_center(Vector3 new_center) {
    Vector3i new_center_chunk = Vector3i();
    new_center_chunk.x = new_center.x - int64_t(new_center.x) % Chunk::CHUNK_SIZE_X;
    new_center_chunk.y = 0;
    new_center_chunk.z = new_center.z - int64_t(new_center.z) % Chunk::CHUNK_SIZE_Z;

    if (new_center_chunk != center_chunk) {
        center_chunk = new_center_chunk;
        update_loaded_region();
    }

    center = new_center;
}

void World::initialize_chunk(uint64_t index) {
    Object::cast_to<Chunk>(initiliazation_queue[index])->generate_data();
    Object::cast_to<Chunk>(initiliazation_queue[index])->generate_mesh();
}

void World::update_loaded_region() {
    /*
    There are two radii relevant to chunk loading:
    - instance_radius (blocks):
    - - If a chunk's distance from the center is < instance_radius, instantiate it and add it to the scene tree;
    - - If the chunk is not in memory, generate it from scratch / storage;
    - - If a chunk is > instance_radius, remove it from the screne tree and place it in memory;
    - unload_radius (blocks):
    - - If a chunk's distance from the center is is > unload_radius, delete it from the scene tree AND memory
    */

    // Unload chunks outside of unload radius
    Array keys = loaded_chunks.keys();
    TypedArray<Vector3i> chunks_to_unload = TypedArray<Vector3i>();
    for (int64_t i = 0; i < keys.size(); i++) {
        if (!is_chunk_in_radius(Object::cast_to<Chunk>(loaded_chunks[keys[i]])->get_position(), unload_radius)) {
            chunks_to_unload.append(keys[i]);
        }
    }
    for (int64_t i = 0; i < chunks_to_unload.size(); i++) {
        Chunk* to_unload = Object::cast_to<Chunk>(loaded_chunks[chunks_to_unload[i]]);
        if (to_unload->is_inside_tree()) {
            to_unload->get_parent()->remove_child(to_unload);
        }
        to_unload->queue_free();
        loaded_chunks.erase(chunks_to_unload[i]);
    }

    // Loop through instanced chunks to remove any that are outside of instance radius
    is_chunk_instanced = Dictionary();
    for (uint64_t i = 0; i < get_child_count(); i++) {
        Chunk* chunk = Object::cast_to<Chunk>(get_child(i));
        if (!is_chunk_in_radius(Vector3i(chunk->get_position()), instance_radius)) {
            i--; // We do not want to skip over children when we remove one
            remove_child(chunk);
        } else {
            is_chunk_instanced[Vector3i(chunk->get_position())] = true;
        }
    }
    // Loop through the circular region around the center and generate chunks
    for (int64_t chunk_x = -(instance_radius / Chunk::CHUNK_SIZE_X); chunk_x <= instance_radius / Chunk::CHUNK_SIZE_X; chunk_x++) {
        for (int64_t chunk_z = -(instance_radius / Chunk::CHUNK_SIZE_Z); chunk_z <= instance_radius / Chunk::CHUNK_SIZE_Z; chunk_z++) {
            Vector3i coordinate = Vector3i(Chunk::CHUNK_SIZE_X * chunk_x, 0, Chunk::CHUNK_SIZE_Z * chunk_z) + center_chunk;
            if (is_chunk_instanced.has(coordinate) || !is_chunk_in_radius(coordinate, instance_radius)) {
                continue;
            }

            if (loaded_chunks.has(coordinate)) {
                add_child(Object::cast_to<Node>(loaded_chunks[coordinate]));
            } else if (!is_chunk_in_queue.has(coordinate)) {
                Chunk* new_chunk = memnew(Chunk);
                new_chunk->set_position(coordinate);
                new_chunk->set_main_noise_texture(main_noise_texture);
                new_chunk->set_block_material(block_material);
                add_child(new_chunk);

                is_chunk_in_queue[coordinate] = true;
                initiliazation_queue.append(new_chunk);
            }
        }
    }
}

void World::generate_from_queue(uint64_t n) {
    for (uint64_t i = 0; i < initiliazation_queue.size(); i++) {
        Chunk* chunk = Object::cast_to<Chunk>(initiliazation_queue[i]);
        Vector3i coordinate = chunk->get_position();
        initialize_chunk(i);

        is_chunk_instanced[coordinate] = true;
        is_chunk_in_queue.erase(coordinate);
    }
    initiliazation_queue = TypedArray<Chunk>();
}

bool World::is_chunk_in_radius(Vector3i coordinate, int64_t radius) {
    Vector3i displacement = center_chunk - coordinate;
    return displacement.x * displacement.x + displacement.z * displacement.z < radius * radius;
}
