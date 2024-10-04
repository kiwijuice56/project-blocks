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

    ClassDB::bind_method(D_METHOD("get_instance_radius"), &World::get_instance_radius);
	ClassDB::bind_method(D_METHOD("set_instance_radius", "new_radius"), &World::set_instance_radius);

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

    ClassDB::add_property(
        "World",
        PropertyInfo(Variant::INT, "instance_radius"),
        "set_instance_radius",
        "get_instance_radius"
    );
}

World::World() {
    initiliazation_queue = TypedArray<Chunk>();
    initiliazation_queue_positions = PackedVector3Array();
}

World::~World() { }

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

void World::set_instance_radius(int64_t new_radius) {
    instance_radius = new_radius;
}

int64_t World::get_instance_radius() const {
    return instance_radius;
}

void World::set_center(Vector3 new_center) {
    Vector3i new_center_chunk = Vector3i();
    new_center_chunk.x = new_center.x - int64_t(new_center.x) % Chunk::CHUNK_SIZE_X;
    new_center_chunk.y = new_center.y - int64_t(new_center.y) % Chunk::CHUNK_SIZE_Y;
    new_center_chunk.z = new_center.z - int64_t(new_center.z) % Chunk::CHUNK_SIZE_Z;

    center_chunk = new_center_chunk;
    center = new_center;

    update_loaded_region();
}

void World::initialize_chunk(uint64_t index) {
    Object::cast_to<Chunk>(initiliazation_queue[index])->generate_data(initiliazation_queue_positions[index]);
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

    if (has_task) {
        if (!WorkerThreadPool::get_singleton()->is_group_task_completed(task_id)) return;
        WorkerThreadPool::get_singleton()->wait_for_group_task_completion(task_id);
    }

    initiliazation_queue.clear();
    initiliazation_queue_positions.clear();

    // Loop through instanced chunks to remove any that are outside of instance radius
    for (uint64_t i = 0; i < get_child_count(); i++) {
        Chunk* chunk = Object::cast_to<Chunk>(get_child(i));
        if (!is_chunk_in_radius(Vector3i(chunk->get_position()), instance_radius)) {
            i--; // We do not want to skip over children when we remove one
            remove_child(chunk);
            chunk->queue_free();
            is_chunk_instanced.erase(Vector3i(chunk->get_position()));
        }
    }

    // Loop through the spherical region around the center and generate chunks
    int64_t min_y = -(instance_radius / Chunk::CHUNK_SIZE_Y);
    int64_t max_y = +(instance_radius / Chunk::CHUNK_SIZE_Y);
    int64_t min_x = -(instance_radius / Chunk::CHUNK_SIZE_X);
    int64_t max_x = +(instance_radius / Chunk::CHUNK_SIZE_X);
    int64_t min_z = -(instance_radius / Chunk::CHUNK_SIZE_Z);
    int64_t max_z = +(instance_radius / Chunk::CHUNK_SIZE_Z);
    for (int64_t chunk_y = min_y; chunk_y <= max_y; chunk_y++) {
        for (int64_t chunk_x = min_x; chunk_x <= max_x; chunk_x++) {
            for (int64_t chunk_z = min_z; chunk_z <= max_z; chunk_z++) {
                Vector3i coordinate = Vector3i(Chunk::CHUNK_SIZE_X * chunk_x, Chunk::CHUNK_SIZE_Y * chunk_y, Chunk::CHUNK_SIZE_Z * chunk_z) + center_chunk;
                if (is_chunk_instanced.has(coordinate) || !is_chunk_in_radius(coordinate, instance_radius)) {
                    continue;
                }

                is_chunk_instanced[coordinate] = true;

                Chunk* new_chunk = memnew(Chunk);
                new_chunk->set_position(coordinate);
                new_chunk->set_main_noise_texture(main_noise_texture);
                new_chunk->set_block_material(block_material);
                add_child(new_chunk);

                initiliazation_queue.append(new_chunk);
                initiliazation_queue_positions.append(coordinate);
            }
        }
    }

    has_task = initiliazation_queue.size() > 0;

    if (has_task) {
        task_id = WorkerThreadPool::get_singleton()->add_group_task(callable_mp(this, &World::initialize_chunk), initiliazation_queue.size());
    }
}

bool World::is_chunk_in_radius(Vector3i coordinate, int64_t radius) {
    Vector3i displacement = center_chunk - coordinate;
    return displacement.length_squared() < radius * radius;
}
