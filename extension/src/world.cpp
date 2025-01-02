#include "../include/world.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

////////////////////////
//   Initialization   //
////////////////////////

void World::_bind_methods() {
    ClassDB::bind_method(D_METHOD("initialize"), &World::initialize);
    ClassDB::bind_method(D_METHOD("set_loaded_region_center", "new_center"), &World::set_loaded_region_center);
    ClassDB::bind_method(D_METHOD("simulate_water"), &World::simulate_water);
    ClassDB::bind_method(D_METHOD("is_position_loaded"), &World::is_position_loaded);

    ClassDB::bind_method(D_METHOD("get_block_types"), &World::get_block_types);
	ClassDB::bind_method(D_METHOD("set_block_types", "new_block_types"), &World::set_block_types);

    ClassDB::bind_method(D_METHOD("get_decorations"), &World::get_decorations);
	ClassDB::bind_method(D_METHOD("set_decorations", "new_block_types"), &World::set_decorations);

    ClassDB::bind_method(D_METHOD("get_block_material"), &World::get_block_material);
	ClassDB::bind_method(D_METHOD("set_block_material", "new_material"), &World::set_block_material);

    ClassDB::bind_method(D_METHOD("get_water_material"), &World::get_water_material);
	ClassDB::bind_method(D_METHOD("set_water_material", "new_material"), &World::set_water_material);

    ClassDB::bind_method(D_METHOD("get_transparent_block_material"), &World::get_transparent_block_material);
	ClassDB::bind_method(D_METHOD("set_transparent_block_material", "new_material"), &World::set_transparent_block_material);

    ClassDB::bind_method(D_METHOD("get_instance_radius"), &World::get_instance_radius);
	ClassDB::bind_method(D_METHOD("set_instance_radius", "new_radius"), &World::set_instance_radius);

    ClassDB::bind_method(D_METHOD("get_generator"), &World::get_generator);
	ClassDB::bind_method(D_METHOD("set_generator", "new_generator"), &World::set_generator);

	ClassDB::bind_method(D_METHOD("get_block_type_at", "position"), &World::get_block_type_at);
    ClassDB::bind_method(D_METHOD("break_block_at", "position", "drop_item", "play_effect"), &World::break_block_at);
    ClassDB::bind_method(D_METHOD("place_block_at", "position", "block_type", "play_effect"), &World::place_block_at);
    ClassDB::bind_method(D_METHOD("place_water_at", "position", "amount"), &World::place_water_at);
    ClassDB::bind_method(D_METHOD("get_water_level_at", "position"), &World::get_water_level);

    ADD_PROPERTY(
        PropertyInfo(
            Variant::ARRAY,
            "block_types",
            PROPERTY_HINT_TYPE_STRING,
            String::num(Variant::OBJECT) + "/" + String::num(PROPERTY_HINT_RESOURCE_TYPE) + ":Block"
        ),
        "set_block_types",
        "get_block_types"
    );

    ADD_PROPERTY(
        PropertyInfo(
            Variant::ARRAY,
            "decorations",
            PROPERTY_HINT_TYPE_STRING,
            String::num(Variant::OBJECT) + "/" + String::num(PROPERTY_HINT_RESOURCE_TYPE) + ":Decoration"
        ),
        "set_decorations",
        "get_decorations"
    );

    ADD_PROPERTY(
        PropertyInfo(
            Variant::OBJECT,
            "generator",
            PROPERTY_HINT_RESOURCE_TYPE,
            "Generator"
        ),
        "set_generator",
        "get_generator"
    );

    ADD_PROPERTY(
        PropertyInfo(
            Variant::OBJECT,
            "block_material",
            PROPERTY_HINT_RESOURCE_TYPE,
            "ShaderMaterial"
        ),
        "set_block_material",
        "get_block_material"
    );

    ADD_PROPERTY(
        PropertyInfo(
            Variant::OBJECT,
            "water_material",
            PROPERTY_HINT_RESOURCE_TYPE,
            "ShaderMaterial"
        ),
        "set_water_material",
        "get_water_material"
    );

    ADD_PROPERTY(
        PropertyInfo(
            Variant::OBJECT,
            "transparent_block_material",
            PROPERTY_HINT_RESOURCE_TYPE,
            "ShaderMaterial"
        ),
        "set_transparent_block_material",
        "get_transparent_block_material"
    );


    ADD_PROPERTY(
        PropertyInfo(Variant::INT, "instance_radius"),
        "set_instance_radius",
        "get_instance_radius"
    );

    ADD_SIGNAL(MethodInfo("block_placed", PropertyInfo(Variant::VECTOR3, "position")));
    ADD_SIGNAL(MethodInfo("block_broken", PropertyInfo(Variant::VECTOR3, "position")));
}

World::World() { }

World::~World() { }

// We need a separate method because of some loading issues in Godot
void World::initialize() {
    dropped_item_scene = ResourceLoader::get_singleton()->load("res://main/items/dropped_item/dropped_item.tscn");
    break_effect_scene = ResourceLoader::get_singleton()->load("res://main/items/blocks/break_effect/break_effect.tscn");
    place_effect_scene = ResourceLoader::get_singleton()->load("res://main/items/blocks/place_effect/place_effect.tscn");

    for (int64_t i = 0; i < block_types.size(); i++) {
        Block* block = Object::cast_to<Block>(block_types[i]);
        block->index = i;
        block_id_to_index_map[block->get_id()] = i;
        block_name_map[block->get_internal_name()] = i;
    }

    for (int64_t i = 0; i < decorations.size(); i++) {
        Ref<Decoration> d = Object::cast_to<Decoration>(decorations[i]);
        decoration_name_map[d->get_internal_name()] = d;
    }

    create_texture_atlas();

    // Do not create chunk children when only in the editor
    if (!Engine::get_singleton()->is_editor_hint()) {
        instantiate_chunks();
    }
}

void World::instantiate_chunks() {
    all_chunks.clear();

    center_chunk = Vector3i(0, 0, 0);
    int64_t max_y = instance_radius / Chunk::CHUNK_SIZE_Y; int64_t min_y = -max_y;
    int64_t max_x = instance_radius / Chunk::CHUNK_SIZE_X; int64_t min_x = -max_x;
    int64_t max_z = instance_radius / Chunk::CHUNK_SIZE_Z; int64_t min_z = -max_z;
    for (int64_t chunk_y = min_y; chunk_y <= max_y; chunk_y++) {
        for (int64_t chunk_x = min_x; chunk_x <= max_x; chunk_x++) {
            for (int64_t chunk_z = min_z; chunk_z <= max_z; chunk_z++) {
                Vector3i coordinate = center_chunk + Vector3i(Chunk::CHUNK_SIZE_X * chunk_x, Chunk::CHUNK_SIZE_Y * chunk_y, Chunk::CHUNK_SIZE_Z * chunk_z);
                if (!is_chunk_in_radius(coordinate, instance_radius)) {
                    continue;
                }

                Chunk* new_chunk = memnew(Chunk);

                new_chunk->set_position(coordinate);
                new_chunk->world = this;

                new_chunk->block_types = block_types;
                new_chunk->block_material = block_material;
                new_chunk->transparent_block_material = transparent_block_material;
                new_chunk->water_mesh->set_material_override(water_material);

                add_child(new_chunk);

                all_chunks.push_back(new_chunk);
            }
        }
    }
}

void World::create_texture_atlas() {
    TypedArray<Image> images;

    images.resize(6 * block_types.size());
    for (int64_t i = 0; i < block_types.size(); i++) {
        Block* block = Object::cast_to<Block>(block_types[i]);
        Ref<Image> combined_image = block->get_texture()->get_image();
        for (int64_t j = 0; j < 6; j++) {
            Ref<Image> side_image = Image::create_empty(16, 16, false, Image::FORMAT_RGBA8);
            side_image->blit_rect(combined_image, Rect2i(j * 16, 0, 16, 16), Vector2i());
            images[6 * i + j] = side_image;
        }
    }

    Ref<Texture2DArray> atlas = memnew(Texture2DArray);
    atlas->create_from_images(images);
    block_material->set_shader_parameter("textures", atlas);
    transparent_block_material->set_shader_parameter("textures", atlas);
}


////////////////////////
//   Chunk Loading    //
////////////////////////


void World::set_loaded_region_center(Vector3 new_center) {
    Vector3i new_center_chunk = Vector3i();
    new_center_chunk.x = new_center.x - int64_t(new_center.x) % Chunk::CHUNK_SIZE_X;
    new_center_chunk.y = new_center.y - int64_t(new_center.y) % Chunk::CHUNK_SIZE_Y;
    new_center_chunk.z = new_center.z - int64_t(new_center.z) % Chunk::CHUNK_SIZE_Z;

    center_chunk = new_center_chunk;
    center = Vector3i(new_center);
    update_loaded_region();
}

void World::update_loaded_region() {
    if (has_task) {
        if (!WorkerThreadPool::get_singleton()->is_group_task_completed(task_id)) {
            return;
        }
        WorkerThreadPool::get_singleton()->wait_for_group_task_completion(task_id);
        has_task = false;
    }

    init_queue.clear();
    init_queue_positions.clear();

    bool all_decorations_generated = true;

    // Generate decorations in a spherical region around the center (one chunk farther than rendered chunks)

    // But first initialize dictionary spots
    int64_t deco_radius_x = 2 + instance_radius / Chunk::CHUNK_SIZE_X;
    int64_t deco_radius_y = 2 + instance_radius / Chunk::CHUNK_SIZE_Y;
    int64_t deco_radius_z = 2 + instance_radius / Chunk::CHUNK_SIZE_Z;
    for (int64_t y = -deco_radius_y; y <= deco_radius_y; y++) {
        for (int64_t x = -deco_radius_x; x <= deco_radius_x; x++) {
            for (int64_t z = -deco_radius_z; z <= deco_radius_z; z++) {
                Vector3i coordinate = Vector3i(Chunk::CHUNK_SIZE_X * x, Chunk::CHUNK_SIZE_Y * y, Chunk::CHUNK_SIZE_Z * z) + center_chunk;

                if (!decoration_map.has(coordinate)) {
                    Array decoration_array;
                    decoration_array.resize(MAX_DECORATIONS);
                    decoration_map[coordinate] = decoration_array;
                    decoration_count[coordinate] = 0;
                }
            }
        }
    }

    deco_radius_x = 1 + instance_radius / Chunk::CHUNK_SIZE_X;
    deco_radius_y = 1 + instance_radius / Chunk::CHUNK_SIZE_Y;
    deco_radius_z = 1 + instance_radius / Chunk::CHUNK_SIZE_Z;
    for (int64_t y = -deco_radius_y; y <= deco_radius_y; y++) {
        for (int64_t x = -deco_radius_x; x <= deco_radius_x; x++) {
            for (int64_t z = -deco_radius_z; z <= deco_radius_z; z++) {
                Vector3i coordinate = Vector3i(Chunk::CHUNK_SIZE_X * x, Chunk::CHUNK_SIZE_Y * y, Chunk::CHUNK_SIZE_Z * z) + center_chunk;

                if (decoration_generated.has(coordinate)) {
                    continue;
                }

                decoration_generated[coordinate] = false;
                all_decorations_generated = false;
                init_queue_positions.push_back(coordinate);
            }
        }
    }

    if (all_decorations_generated) {
        std::vector<Chunk*> available_chunks;

        // Loop through chunks to remove any that are outside of instance radius
        for (uint64_t i = 0; i < all_chunks.size(); i++) {
            Chunk* chunk = all_chunks[i];
            Vector3i coordinate = Vector3i(chunk->get_position());
            if (chunk->never_initialized || !is_chunk_in_radius(coordinate, instance_radius)) {
                chunk_map.erase(coordinate);
                is_chunk_loaded.erase(coordinate);
                available_chunks.push_back(chunk);

                if (chunk->modified) {
                    chunk_data[coordinate] = chunk->blocks;
                    chunk_water_data[coordinate] = chunk->water;
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

                    init_queue.push_back(new_chunk);
                    init_queue_positions.push_back(coordinate);
                    is_chunk_loaded[coordinate] = false;

                    chunk_map[coordinate] = new_chunk;
                }
            }
        }

        if (init_queue.size() > 0) {
            has_task = true;
            task_id = WorkerThreadPool::get_singleton()->add_group_task(callable_mp(this, &World::initialize_chunk), init_queue.size());
        }
    } else {
        if (init_queue_positions.size() > 0) {
            has_task = true;
            task_id = WorkerThreadPool::get_singleton()->add_group_task(callable_mp(this, &World::initialize_chunk_decorations), init_queue_positions.size());
        }
    }
}

void World::initialize_chunk(uint64_t index) {
    Chunk* chunk = Object::cast_to<Chunk>(init_queue[index]);
    Vector3i coordinate = init_queue_positions[index];

    if (chunk_data.has(coordinate)) {
        chunk->blocks = chunk_data[coordinate];
        chunk->water = chunk_water_data[coordinate];
    } else {
        generator->generate_terrain_blocks(this, chunk, decoration_map[coordinate], coordinate);
        generator->generate_decoration_blocks(this, chunk, decoration_map[coordinate], coordinate);
        generator->generate_water(this, chunk, decoration_map[coordinate], coordinate);
    }

    chunk->calculate_block_statistics();
    chunk->never_initialized = false;
    chunk->generate_mesh(false);
    chunk->generate_water_mesh();

    // Ensure water isn't asleep when a chunk is loaded
    if (chunk->water_count > 0) {
        chunk->water_chunk_awake.fill(2);
    }

    is_chunk_loaded[coordinate] = true;
}

void World::initialize_chunk_decorations(uint64_t index) {
    Vector3i coordinate = init_queue_positions[index];
    generator->generate_decorations(this, coordinate);
    decoration_generated[coordinate] = true;
}


//////////////////////////////
//   Interfacing Methods    //
//////////////////////////////


void World::simulate_water() {
    int64_t chunk_radius_x = water_simulate_radius / Chunk::CHUNK_SIZE_X;
    int64_t chunk_radius_y = water_simulate_radius / Chunk::CHUNK_SIZE_Y;
    int64_t chunk_radius_z = water_simulate_radius / Chunk::CHUNK_SIZE_Z;

    for (int64_t y = -chunk_radius_y; y <= chunk_radius_y; y++) {
        for (int64_t z = -chunk_radius_z; z <= chunk_radius_z; z++) {
            for (int64_t x = -chunk_radius_x; x <= chunk_radius_x; x++) {
                Vector3i coordinate = Vector3i(Chunk::CHUNK_SIZE_X * x, Chunk::CHUNK_SIZE_Y * y, Chunk::CHUNK_SIZE_Z * z) + center_chunk;
                if ((int64_t)UtilityFunctions::abs(x + y + z)% 3 != water_frame) {
                    continue;
                }
                if (!is_chunk_loaded.has(coordinate) || !is_chunk_in_radius(coordinate, water_simulate_radius)) {
                    continue;
                }

                Chunk* chunk = Object::cast_to<Chunk>(chunk_map[coordinate]);
                chunk->simulate_water();
            }
        }
    }
    for (int64_t y = -chunk_radius_y; y <= chunk_radius_y; y++) {
        for (int64_t z = -chunk_radius_z; z <= chunk_radius_z; z++) {
            for (int64_t x = -chunk_radius_x; x <= chunk_radius_x; x++) {
                if ((int64_t)UtilityFunctions::abs(x + y + z) % 3 != water_frame) {
                    continue;
                }

                Vector3i coordinate = Vector3i(Chunk::CHUNK_SIZE_X * x, Chunk::CHUNK_SIZE_Y * y, Chunk::CHUNK_SIZE_Z * z) + center_chunk;
                if (!is_chunk_loaded.has(coordinate) || !is_chunk_in_radius(coordinate, water_simulate_radius)) {
                    continue;
                }

                Chunk* chunk = Object::cast_to<Chunk>(chunk_map[coordinate]);
                if (chunk->water_updated) {
                    chunk->generate_water_mesh();
                    chunk->water_updated = false;
                }
            }
        }
    }

    water_frame = (water_frame + 1) % 3;
}

void World::place_decoration(Ref<Decoration> decoration, Vector3i position) {
    Ref<Decoration> new_decoration = memnew(Decoration);
    new_decoration->position = position;
    new_decoration->set_blocks(decoration->get_blocks());
    new_decoration->set_size(decoration->get_size());

    Dictionary placed_chunks;
    for (int64_t i = 0; i <= 1; i++) {
        for (int64_t j = 0; j <= 1; j++) {
            for (int64_t k = 0; k <= 1; k++) {
                Vector3i corner = position + new_decoration->get_size() * Vector3i(i, j, k);
                Vector3i chunk_position = snap_to_chunk(corner);

                if (!placed_chunks.has(chunk_position)) {
                    decoration_lock.lock();

                    int64_t count = decoration_count[chunk_position];

                    if (count >= MAX_DECORATIONS) {
                        decoration_lock.unlock();
                        continue;
                    }

                    Array decoration_list = decoration_map[chunk_position];
                    decoration_list[count] = new_decoration;
                    decoration_count[chunk_position] = count + 1;

                    decoration_lock.unlock();

                    placed_chunks[chunk_position] = true;
                }
            }
        }
    }
}

bool World::is_position_loaded(Vector3 position) {
    position = position.floor();

    Vector3i snapped_position = snap_to_chunk(position);
    return is_chunk_loaded.has(snapped_position) && is_chunk_loaded[snapped_position];
}

// Assumes the chunk is loaded
Chunk* World::get_chunk_at(Vector3 position) {
    position = position.floor();

    return Object::cast_to<Chunk>(chunk_map[snap_to_chunk(position)]);
}

Vector3i World::snap_to_chunk(Vector3 position) {
    position = position.floor();

    Vector3i p = Vector3i(position);

    int64_t rx = int64_t(p.x) % Chunk::CHUNK_SIZE_X;
    int64_t ry = int64_t(p.y) % Chunk::CHUNK_SIZE_Y;
    int64_t rz = int64_t(p.z) % Chunk::CHUNK_SIZE_Z;

    if (rx < 0) rx += Chunk::CHUNK_SIZE_X;
    if (ry < 0) ry += Chunk::CHUNK_SIZE_Y;
    if (rz < 0) rz += Chunk::CHUNK_SIZE_Z;

    return p - Vector3i(rx, ry, rz);
}

bool World::is_chunk_in_radius(Vector3i coordinate, int64_t radius) {
    return (center_chunk - coordinate).length_squared() < radius * radius;
}

Ref<Block> World::get_block_type_at(Vector3 position) {
    position = position.floor();

    Chunk* chunk = get_chunk_at(position);
    return block_types[chunk->get_block_index_at_global(Vector3i(position))];
}

void World::break_block_at(Vector3 position, bool drop_item, bool play_effect) {
    position = position.floor();

    Chunk* chunk = get_chunk_at(position);
    Ref<Block> block_type = get_block_type_at(position);
    chunk->remove_block_at(Vector3i(position));

    if (drop_item && block_type->get_can_drop()) {
        Node* dropped_item = dropped_item_scene->instantiate();
        get_parent()->add_child(dropped_item);
        dropped_item->set("global_position", Vector3i(position));
        dropped_item->set("world", this);
        Ref<ItemState> new_item = memnew(ItemState);
        new_item->set_id((block_type->get_drop_item() != nullptr ? block_type->get_drop_item() : block_type)->get_id());
        new_item->set_count(1);
        dropped_item->call("initialize", new_item);
    }

    if (play_effect) {
        Node* break_effect = break_effect_scene->instantiate();
        get_parent()->add_child(break_effect);
        break_effect->set("global_position", Vector3i(position));
        break_effect->call("initialize", block_type);
    }

    emit_signal("block_broken", position);
}

void World::place_block_at(Vector3 position, Ref<Block> block_type, bool play_effect) {
    position = position.floor();

    Chunk* chunk = get_chunk_at(position);
    chunk->place_block_at(Vector3i(position), block_type->index);

    if (play_effect) {
        Node* place_effect = place_effect_scene->instantiate();
        get_parent()->add_child(place_effect);
        place_effect->set("global_position", Vector3i(position));
        place_effect->call("initialize", block_type);
    }

    emit_signal("block_placed", Vector3i(position));
}

void World::place_water_at(Vector3 position, uint8_t amount) {
    position = position.floor();

    Chunk* chunk = get_chunk_at(position);
    chunk->set_water_at(Vector3i(position - chunk->get_global_position()), amount);
}

uint8_t World::get_water_level(Vector3 position) {
    position = position.floor();
    Chunk* chunk = get_chunk_at(position);
    return chunk->get_water_at(Vector3i(position - chunk->get_global_position()));
}


///////////////////////////////////
//   Boilerplate setter/getter   //
//////////////////////////////////


void World::set_block_types(TypedArray<Block> new_block_types) {
    block_types = new_block_types;
}

TypedArray<Block> World::get_block_types() const {
    return block_types;
}

Ref<ShaderMaterial> World::get_block_material() const {
    return block_material;
}

void World::set_block_material(Ref<ShaderMaterial> new_material) {
    block_material = new_material;
}

Ref<ShaderMaterial> World::get_water_material() const {
    return water_material;
}

void World::set_water_material(Ref<ShaderMaterial> new_material) {
    water_material = new_material;
}

Ref<ShaderMaterial> World::get_transparent_block_material() const {
    return transparent_block_material;
}

void World::set_transparent_block_material(Ref<ShaderMaterial> new_material) {
    transparent_block_material = new_material;
}

void World::set_instance_radius(int64_t new_radius) {
    instance_radius = new_radius;
}

int64_t World::get_instance_radius() const {
    return instance_radius;
}

TypedArray<Decoration> World::get_decorations() const {
    return decorations;
}

void World::set_decorations(TypedArray<Decoration> new_decorations) {
    decorations = new_decorations;
}

Ref<Generator> World::get_generator() const {
    return generator;
}

void World::set_generator(Ref<Generator> new_generator) {
    generator = new_generator;
}
