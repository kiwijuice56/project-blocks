#include "../include/world.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void World::_bind_methods() {
    ClassDB::bind_method(D_METHOD("initialize"), &World::initialize);
    ClassDB::bind_method(D_METHOD("set_loaded_region_center", "new_center"), &World::set_loaded_region_center);
    ClassDB::bind_method(D_METHOD("is_position_loaded"), &World::is_position_loaded);
    ClassDB::bind_method(D_METHOD("get_chunk_at"), &World::get_chunk_at);

    ClassDB::bind_method(D_METHOD("get_block_types"), &World::get_block_types);
	ClassDB::bind_method(D_METHOD("set_block_types", "new_block_types"), &World::set_block_types);

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

World::World() { }

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

void World::set_instance_radius(int64_t new_radius) {
    instance_radius = new_radius;
}

int64_t World::get_instance_radius() const {
    return instance_radius;
}

void World::set_loaded_region_center(Vector3 new_center) {
    Vector3i new_center_chunk = Vector3i();
    new_center_chunk.x = new_center.x - int64_t(new_center.x) % Chunk::CHUNK_SIZE_X;
    new_center_chunk.y = new_center.y - int64_t(new_center.y) % Chunk::CHUNK_SIZE_Y;
    new_center_chunk.z = new_center.z - int64_t(new_center.z) % Chunk::CHUNK_SIZE_Z;

    center_chunk = new_center_chunk;
    update_loaded_region();
}

void World::initialize() {
    // Do not create chunk children when only in the editor
    if (!Engine::get_singleton()->is_editor_hint()) {
        regenerate_chunks();
    }
}

void World::regenerate_chunks() {
    all_chunks.clear();

    center_chunk = Vector3i(0, 0, 0);
    int64_t max_y = instance_radius / Chunk::CHUNK_SIZE_Y; int64_t min_y = -max_y;
    int64_t max_x = instance_radius / Chunk::CHUNK_SIZE_X; int64_t min_x = -max_x;
    int64_t max_z = instance_radius / Chunk::CHUNK_SIZE_Z; int64_t min_z = -max_z;
    for (int64_t chunk_y = min_y; chunk_y <= max_y; chunk_y++) {
        for (int64_t chunk_x = min_x; chunk_x <= max_x; chunk_x++) {
            for (int64_t chunk_z = min_z; chunk_z <= max_z; chunk_z++) {
                Vector3i coordinate = center_chunk + Vector3i(Chunk::CHUNK_SIZE_X * chunk_x, Chunk::CHUNK_SIZE_Y * chunk_y, Chunk::CHUNK_SIZE_Z * chunk_z);
                if (!is_chunk_in_radius(coordinate, instance_radius))
                    continue;


                Chunk* new_chunk = memnew(Chunk);

                new_chunk->set_position(coordinate);
                new_chunk->main_noise_texture = main_noise_texture;
                new_chunk->set_material_override(block_material);

                add_child(new_chunk);

                all_chunks.push_back(new_chunk);
            }
        }
    }
}

void World::initialize_chunk(uint64_t index) {
    Chunk* chunk = Object::cast_to<Chunk>(initiliazation_queue[index]);
    Vector3i coordinate = initiliazation_queue_positions[index];
    if (chunk_data.has(coordinate)) {
        chunk->blocks = chunk_data[coordinate];
        chunk->generate_data(initiliazation_queue_positions[index], false);
    } else {
        chunk->generate_data(initiliazation_queue_positions[index], true);
    }

    chunk->generate_mesh();
    is_chunk_loaded[coordinate] = true;
}

void World::update_loaded_region() {
    if (has_task) {
        if (!WorkerThreadPool::get_singleton()->is_group_task_completed(task_id)) {
            return;
        }
        WorkerThreadPool::get_singleton()->wait_for_group_task_completion(task_id);
        has_task = false;
    }

    initiliazation_queue.clear();
    initiliazation_queue_positions.clear();

    std::vector<Chunk*> available_chunks;

    // Loop through chunks to remove any that are outside of instance radius
    for (uint64_t i = 0; i < all_chunks.size(); i++) {
        Chunk* chunk = all_chunks[i];
        Vector3i coordinate = Vector3i(chunk->get_position());
        if (!is_chunk_in_radius(coordinate, instance_radius)) {
            chunk_map.erase(coordinate);
            is_chunk_loaded.erase(coordinate);
            available_chunks.push_back(chunk);

            if (chunk->modified) {
                chunk_data[coordinate] = chunk->blocks;
            }
        }
    }

    // Loop through the spherical region around the center and generate chunks
    // Use a different ordering to generate closer chunks first -- helps reduce jarring loading barriers
    for (int64_t chunk_y = 0; chunk_y <= 2 * instance_radius / Chunk::CHUNK_SIZE_Y; chunk_y++) {
        int64_t actual_chunk_y = (chunk_y % 2 == 0) ? -chunk_y / 2 : (chunk_y + 1) / 2;

        for (int64_t chunk_x = 0; chunk_x <= 2 * instance_radius / Chunk::CHUNK_SIZE_X; chunk_x++) {
            int64_t actual_chunk_x = (chunk_x % 2 == 0) ? -chunk_x / 2 : (chunk_x + 1) / 2;

            for (int64_t chunk_z = 0; chunk_z <= 2 * instance_radius / Chunk::CHUNK_SIZE_Z; chunk_z++) {
                int64_t actual_chunk_z = (chunk_z % 2 == 0) ? -chunk_z / 2 : (chunk_z + 1) / 2;

                Vector3i coordinate = Vector3i(
                    Chunk::CHUNK_SIZE_X * actual_chunk_x,
                    Chunk::CHUNK_SIZE_Y * actual_chunk_y,
                    Chunk::CHUNK_SIZE_Z * actual_chunk_z) + center_chunk;

                if (is_chunk_loaded.has(coordinate) || !is_chunk_in_radius(coordinate, instance_radius)) {
                    continue;
                }

                Chunk* new_chunk = available_chunks.back();
                available_chunks.pop_back();

                new_chunk->set_visible(false);
                new_chunk->set_position(coordinate);
                new_chunk->clear_collision();

                initiliazation_queue.push_back(new_chunk);
                initiliazation_queue_positions.push_back(coordinate);
                is_chunk_loaded[coordinate] = false;

                chunk_map[coordinate] = new_chunk;
            }
        }
    }

    if (initiliazation_queue.size() > 0) {
        has_task = true;
        task_id = WorkerThreadPool::get_singleton()->add_group_task(callable_mp(this, &World::initialize_chunk), initiliazation_queue.size());
    }
}

bool World::is_position_loaded(Vector3 position) {
    Vector3i snapped_position = Vector3i();
    snapped_position.x = position.x - int64_t(position.x) % Chunk::CHUNK_SIZE_X;
    snapped_position.y = position.y - int64_t(position.y) % Chunk::CHUNK_SIZE_Y;
    snapped_position.z = position.z - int64_t(position.z) % Chunk::CHUNK_SIZE_Z;

    return is_chunk_loaded.has(snapped_position) && is_chunk_loaded[snapped_position];
}

// Assumes the chunk is loaded!
Chunk* World::get_chunk_at(Vector3i position) {
    return Object::cast_to<Chunk>(chunk_map[position]);
}

bool World::is_chunk_in_radius(Vector3i coordinate, int64_t radius) {
    return (center_chunk - coordinate).length_squared() < radius * radius;
}
