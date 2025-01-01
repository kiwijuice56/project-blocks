#include "../include/world.h"
#include "../include/chunk.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;


////////////////////////
//   Initialization   //
////////////////////////


void Chunk::_bind_methods() {

}

Chunk::Chunk() {
    // Initialize all data arrays
    visited = new bool[CHUNK_SIZE_X * CHUNK_SIZE_Z * CHUNK_SIZE_Y];
    blocks.resize(CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z);
    water.resize(CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z);
    water_buffer.resize(CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z);
    water_chunk_awake.resize(WATER_CHUNK_SIZE_X * WATER_CHUNK_SIZE_Y * WATER_CHUNK_SIZE_Z);

    // Initialize water mesh (separate child)
    water_mesh = memnew(MeshInstance3D);
    add_child(water_mesh);

    // Initialize collision data
    shape_data_opaque = memnew(ConcavePolygonShape3D);

    CollisionShape3D* collision_shape_opaque = memnew(CollisionShape3D);
    collision_shape_opaque->set_shape(shape_data_opaque);

    StaticBody3D* static_body_opaque = memnew(StaticBody3D);
    static_body_opaque->call_deferred("add_child", collision_shape_opaque);
    add_child(static_body_opaque);

    shape_data_transparent = memnew(ConcavePolygonShape3D);

    CollisionShape3D* collision_shape_transparent = memnew(CollisionShape3D);
    collision_shape_transparent->set_shape(shape_data_transparent);

    StaticBody3D* static_body_transparent = memnew(StaticBody3D);
    static_body_transparent->call_deferred("add_child", collision_shape_transparent);
    add_child(static_body_transparent);
}

Chunk::~Chunk() {
    delete [] visited;
}


//////////////////////////////////////
//   Internal Interfacing Methods   //
//////////////////////////////////////


// Uses local position
uint64_t Chunk::get_block_index_at(Vector3i position) {
    return blocks[position_to_index(position)];
}

uint64_t Chunk::position_to_index(Vector3i position) {
    return uint64_t(position.x) + uint64_t(position.z) * CHUNK_SIZE_X + uint64_t(position.y) * CHUNK_SIZE_Z * CHUNK_SIZE_X;
}

Vector3i Chunk::index_to_position(uint64_t index) {
    return Vector3i(index % CHUNK_SIZE_X, index / (CHUNK_SIZE_X * CHUNK_SIZE_Z), (index / CHUNK_SIZE_X) % CHUNK_SIZE_Z);
}

bool Chunk::in_bounds(Vector3i position) {
    return
    0 <= position.x && position.x < CHUNK_SIZE_X &&
    0 <= position.y && position.y < CHUNK_SIZE_Y &&
    0 <= position.z && position.z < CHUNK_SIZE_Z;
}


//////////////////////////////////////
//   External Interfacing Methods   //
//////////////////////////////////////


uint64_t Chunk::get_block_index_at_global(Vector3i g_position) {
    g_position -= Vector3i(get_global_position());
    return blocks[uint64_t(g_position.x) + uint64_t(g_position.z) * CHUNK_SIZE_X + uint64_t(g_position.y) * CHUNK_SIZE_Z * CHUNK_SIZE_X];
}

void Chunk::remove_block_at(Vector3i global_position) {
    Vector3i block_position = global_position - Vector3i(get_global_position());
    uint64_t index = position_to_index(block_position);

    if (blocks[index] == 0) {
        return;
    }

    water_chunk_wake_set(block_position, true);

    blocks[index] = 0;
    block_count--;
    modified = true;

    generate_mesh(true);
}

void Chunk::place_block_at(Vector3i global_position, uint32_t block_index) {
    Vector3i block_position = global_position - Vector3i(get_global_position());
    uint64_t index = position_to_index(block_position);

    if (blocks[index] != 0) {
        return;
    }

    water_chunk_wake_set(block_position, true);

    blocks[index] = block_index;
    block_count++;
    modified = true;

    generate_mesh(true);
}


/////////////////////////////////
//   Mesh Generation Methods   //
/////////////////////////////////


void Chunk::generate_mesh(bool immediate) {
    Ref<ArrayMesh> array_mesh(memnew(ArrayMesh));
    int material_idx = 0;

    // First pass (opaque objects)
    vertices.clear();
    normals.clear();
    uvs.clear();
    uvs2.clear();

    greedy_mesh_generation(false, false);

    // Package data into an ArrayMesh
    Array arrays;
    arrays.resize(ArrayMesh::ARRAY_MAX);
    arrays[ArrayMesh::ARRAY_VERTEX] = vertices;
    arrays[ArrayMesh::ARRAY_NORMAL] = normals;
    arrays[ArrayMesh::ARRAY_TEX_UV] = uvs;
    arrays[ArrayMesh::ARRAY_TEX_UV2] = uvs2;

    if (vertices.size() > 0) {
        array_mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, arrays);

        if (immediate) {
            shape_data_opaque->set_faces(vertices);
        } else {
            shape_data_opaque->call_deferred("set_faces", vertices);
        }

        array_mesh->surface_set_material(material_idx, block_material);
        material_idx += 1;
    } else {
        if (immediate) {
            shape_data_opaque->set_faces(PackedVector3Array());
        } else {
            shape_data_opaque->call_deferred("set_faces", PackedVector3Array());
        }
    }

    // Second pass (transparent objects)
    vertices.clear();
    normals.clear();
    uvs.clear();
    uvs2.clear();

    greedy_mesh_generation(true, false);

    arrays[ArrayMesh::ARRAY_VERTEX] = vertices;
    arrays[ArrayMesh::ARRAY_NORMAL] = normals;
    arrays[ArrayMesh::ARRAY_TEX_UV] = uvs;
    arrays[ArrayMesh::ARRAY_TEX_UV2] = uvs2;

    if (vertices.size() > 0) {
        array_mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, arrays);

        if (immediate) {
            shape_data_transparent->set_faces(vertices);
        } else {
            shape_data_transparent->call_deferred("set_faces", vertices);
        }

        array_mesh->surface_set_material(material_idx, transparent_block_material);
        material_idx += 1;
    } else {
        if (immediate) {
            shape_data_transparent->set_faces(PackedVector3Array());
        } else {
            shape_data_transparent->call_deferred("set_faces", PackedVector3Array());
        }
    }

    call_deferred("set_mesh", array_mesh);
    call_deferred("set_visible", true);
}

void Chunk::generate_water_mesh() {
    Ref<ArrayMesh> array_mesh(memnew(ArrayMesh));

    vertices.clear();
    normals.clear();
    uvs.clear();
    uvs2.clear();

    greedy_mesh_generation(false, true);

    Array arrays;
    arrays.resize(ArrayMesh::ARRAY_MAX);

    arrays[ArrayMesh::ARRAY_VERTEX] = vertices;
    arrays[ArrayMesh::ARRAY_NORMAL] = normals;

    if (vertices.size() > 0) {
        array_mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, arrays);
    }

    water_mesh->call_deferred("set_mesh", array_mesh);
    water_mesh->call_deferred("set_visible", true);
}

void Chunk::clear_collision() {
    shape_data_opaque->call_deferred("set_faces", PackedVector3Array());
    shape_data_transparent->call_deferred("set_faces", PackedVector3Array());
}

// Keep track of block count and other state
void Chunk::calculate_block_statistics() {
    uint8_t main_block_type = blocks[0];
    uniform = true;
    block_count = 0;
    water_count = 0;
    for (uint64_t i = 0; i < blocks.size(); i++) {
        uniform = blocks[i] == main_block_type;
        if (blocks[i] > 0) {
            block_count++;
        }
    }
    for (uint64_t i = 0; i < water.size(); i++) {
        water_count += water[i];
    }
}


////////////////////////////////
//    Greedy Meshing Logic    //
////////////////////////////////


// Fill vertex, normal, and uv arrays with proper triangles (using the greedy meshing algorithm)
void Chunk::greedy_mesh_generation(bool transparent, bool water_pass) {
    for (uint64_t i = 0; i < CHUNK_SIZE_X * CHUNK_SIZE_Z * CHUNK_SIZE_Y; i++) {
        visited[i] = false;
    }

    face_count = 0;

    for (uint64_t y = 0; y < CHUNK_SIZE_Y; y++) {
        for (uint64_t z = 0; z < CHUNK_SIZE_Z; z++) {
            for (uint64_t x = 0; x < CHUNK_SIZE_Y; x++) {
                if (!water_pass) {
                    current_greedy_block = get_block_index_at(Vector3i(x, y, z));

                    bool block_transparent = Object::cast_to<Block>(block_types[current_greedy_block])->get_transparent();

                    if (block_transparent != transparent)  {
                        continue;
                    }
                } else {
                    current_greedy_block = water[position_to_index(Vector3i(x, y, z))];
                }

                if (greedy_invalid(Vector3i(x, y, z), water_pass)) {
                    continue;
                }

                Vector3i start = Vector3i(x, y, z);
                Vector3 size;

                if (water_pass) {
                    uint8_t water_level = water[position_to_index(Vector3i(x, y, z))];
                    if (water_level == 255) {
                        size = greedy_scan(start, water_pass);
                    } else {
                        size = Vector3(1, water_level / 255., 1);
                    }
                } else if (transparent) {
                    size = Vector3(1, 1, 1);
                } else {
                    size = greedy_scan(start, water_pass);
                }

                add_rectangular_prism(start, size);
            }
        }
    }
}

// Greedily find the size of the largest prism we can add to our mesh
Vector3i Chunk::greedy_scan(Vector3i start, bool water_pass) {
    Vector3i size = Vector3i(1, 1, 1);
    while (!greedy_invalid(start + Vector3i(size.x, 0, 0), water_pass)) {
        visited[position_to_index(start + Vector3i(size.x, 0, 0))] = true;
        size.x++;
    }

    bool axis_done = false;
    while (!axis_done) {
        for (uint8_t x = 0; x < uint8_t(size.x); x++) {
            if (greedy_invalid(start + Vector3i(x, 0, size.z), water_pass)) {
                axis_done = true;
                break;
            }
        }
        if (axis_done) break;
        for (uint8_t x = 0; x < uint8_t(size.x); x++) {
            visited[position_to_index(start + Vector3i(x, 0, size.z))] = true;
        }
        size.z++;
    }

    axis_done = false;
    while (!axis_done) {
        for (uint8_t x = 0; x < uint8_t(size.x); x++) {
            for (uint8_t z = 0; z < uint8_t(size.z); z++) {
                if (greedy_invalid(start + Vector3i(x, size.y, z), water_pass)) {
                    axis_done = true;
                    break;
                }
            }
        }
        if (axis_done) break;
        for (uint8_t x = 0; x < uint8_t(size.x); x++) {
            for (uint8_t z = 0; z < uint8_t(size.z); z++) {
                visited[position_to_index(start + Vector3i(x, size.y, z))] = true;
            }
        }
        size.y++;
    }
    return size;
}

// Check if a position contains a block that can be merged with our current greedy scan
bool Chunk::greedy_invalid(Vector3i position, bool water_pass) {
    if (!in_bounds(position) || visited[position_to_index(position)]) {
        return true;
    }

    if (water_pass) {
        uint8_t water_level = water[position_to_index(position)];

        if (water_level == 0) {
            return true;
        }

        return water_level != current_greedy_block;
    } else {
        uint64_t block_id = get_block_index_at(position);

        if (block_id == 0) {
            return true;
        }

        if (block_id == current_greedy_block) {
            return false;
        }

        bool fully_covered =
            in_bounds(Vector3i(+1, 0, 0) + position) && !Object::cast_to<Block>(block_types[get_block_index_at(Vector3i(+1, 0, 0) + position)])->get_transparent() &&
            in_bounds(Vector3i(-1, 0, 0) + position) && !Object::cast_to<Block>(block_types[get_block_index_at(Vector3i(-1, 0, 0) + position)])->get_transparent() &&
            in_bounds(Vector3i(0, +1, 0) + position) && !Object::cast_to<Block>(block_types[get_block_index_at(Vector3i(0, +1, 0) + position)])->get_transparent() &&
            in_bounds(Vector3i(0, -1, 0) + position) && !Object::cast_to<Block>(block_types[get_block_index_at(Vector3i(0, -1, 0) + position)])->get_transparent() &&
            in_bounds(Vector3i(0, 0, +1) + position) && !Object::cast_to<Block>(block_types[get_block_index_at(Vector3i(0, 0, +1) + position)])->get_transparent() &&
            in_bounds(Vector3i(0, 0, -1) + position) && !Object::cast_to<Block>(block_types[get_block_index_at(Vector3i(0, 0, -1) + position)])->get_transparent();

        return !fully_covered;
    }
}

// Adds vertices, uvs, and normals for a rectangular prism to our mesh
void Chunk::add_rectangular_prism(Vector3 start, Vector3 size) {
    vertices.resize(vertices.size() + 36);
    normals.resize(normals.size() + 36);
    uvs.resize(uvs.size() + 36);
    uvs2.resize(uvs2.size() + 36);

    // Y, facing up
    vertices[face_count * 6 + 0] = start + Vector3(0, size.y, size.z);
    vertices[face_count * 6 + 1] = start + Vector3(0, size.y, 0);
    vertices[face_count * 6 + 2] = start + Vector3(size.x, size.y, 0);
    vertices[face_count * 6 + 3] = start + Vector3(0, size.y, size.z);
    vertices[face_count * 6 + 4] = start + Vector3(size.x, size.y, 0);
    vertices[face_count * 6 + 5] = start + Vector3(size.x, size.y, size.z);
    add_face_normals(Vector3(0, 1, 0));
    add_face_uvs(current_greedy_block * 6, Vector2i(size.x, size.z));

    face_count++;

    // Y, facing down
    vertices[face_count * 6 + 0] = start + Vector3(0, 0, 0);
    vertices[face_count * 6 + 1] = start + Vector3(0, 0, size.z);
    vertices[face_count * 6 + 2] = start + Vector3(size.x, 0, size.z);
    vertices[face_count * 6 + 3] = start + Vector3(0, 0, 0);
    vertices[face_count * 6 + 4] = start + Vector3(size.x, 0, size.z);
    vertices[face_count * 6 + 5] = start + Vector3(size.x, 0, 0);
    add_face_normals(Vector3(0, -1, 0));
    add_face_uvs(current_greedy_block * 6 + 1, Vector2i(size.x, size.z));

    face_count++;

    // Z, facing back
    vertices[face_count * 6 + 0] = start + Vector3(size.x, 0, 0);
    vertices[face_count * 6 + 1] = start + Vector3(size.x, size.y, 0);
    vertices[face_count * 6 + 2] = start + Vector3(0, size.y, 0);
    vertices[face_count * 6 + 3] = start + Vector3(size.x, 0, 0);
    vertices[face_count * 6 + 4] = start + Vector3(0, size.y, 0);
    vertices[face_count * 6 + 5] = start + Vector3(0, 0, 0);
    add_face_normals(Vector3(0, 0, -1));
    add_face_uvs(current_greedy_block * 6 + 2, Vector2i(size.x, size.y));

    face_count++;

    // Z, facing forward
    vertices[face_count * 6 + 0] = start + Vector3(0, 0, size.z);
    vertices[face_count * 6 + 1] = start + Vector3(0, size.y, size.z);
    vertices[face_count * 6 + 2] = start + Vector3(size.x, size.y, size.z);
    vertices[face_count * 6 + 3] = start + Vector3(0, 0, size.z);
    vertices[face_count * 6 + 4] = start + Vector3(size.x, size.y, size.z);
    vertices[face_count * 6 + 5] = start + Vector3(size.x, 0, size.z);
    add_face_normals(Vector3(0, 0, 1));
    add_face_uvs(current_greedy_block * 6 + 3, Vector2i(size.x, size.y));

    face_count++;

    // X, facing left
    vertices[face_count * 6 + 0] = start + Vector3(0, 0, 0);
    vertices[face_count * 6 + 1] = start + Vector3(0, size.y, 0);
    vertices[face_count * 6 + 2] = start + Vector3(0, size.y, size.z);
    vertices[face_count * 6 + 3] = start + Vector3(0, 0, 0);
    vertices[face_count * 6 + 4] = start + Vector3(0, size.y, size.z);
    vertices[face_count * 6 + 5] = start + Vector3(0, 0, size.z);
    add_face_normals(Vector3(-1, 0, 0));
    add_face_uvs(current_greedy_block * 6 + 4, Vector2i(size.z, size.y));

    face_count++;

    // X, facing right
    vertices[face_count * 6 + 0] = start + Vector3(size.x, 0, size.z);
    vertices[face_count * 6 + 1] = start + Vector3(size.x, size.y, size.z);
    vertices[face_count * 6 + 2] = start + Vector3(size.x, size.y, 0);
    vertices[face_count * 6 + 3] = start + Vector3(size.x, 0, size.z);
    vertices[face_count * 6 + 4] = start + Vector3(size.x, size.y, 0);
    vertices[face_count * 6 + 5] = start + Vector3(size.x, 0, 0);
    add_face_normals(Vector3(1, 0, 0));
    add_face_uvs(current_greedy_block * 6 + 5, Vector2i(size.z, size.y));

    face_count++;
}

void Chunk::add_face_uvs(uint64_t id, Vector2i scale) {
    uvs[face_count * 6 + 0] = scale * Vector2i(0, 1);
	uvs[face_count * 6 + 1] = scale * Vector2i(0, 0);
	uvs[face_count * 6 + 2] = scale * Vector2i(1, 0);
	uvs[face_count * 6 + 3] = scale * Vector2i(0, 1);
    uvs[face_count * 6 + 4] = scale * Vector2i(1, 0);
	uvs[face_count * 6 + 5] = scale * Vector2i(1, 1);
    for (uint8_t i = 0; i < 6; i++) uvs2[face_count * 6 + i] = Vector2i(0, id);
}

void Chunk::add_face_normals(Vector3i normal) {
    for (uint8_t i = 0; i < 6; i++) normals[face_count * 6 + i] = normal;
}


////////////////////////////////
//   Water Simulation Logic   //
////////////////////////////////


void Chunk::water_chunk_wake_set(Vector3i local_position, bool awake) {
    Vector3i water_chunk_coord = local_position / Vector3i(WATER_CHUNK_SIZE_X, WATER_CHUNK_SIZE_Y, WATER_CHUNK_SIZE_Z);
    water_chunk_awake[uint64_t(water_chunk_coord.x) + uint64_t(water_chunk_coord.z) * WATER_CHUNK_SIZE_X + uint64_t(water_chunk_coord.y) * WATER_CHUNK_SIZE_X * WATER_CHUNK_SIZE_Z] = awake ? 1 : 0;
    // TODO: waken surrounding chunks
}

bool Chunk::is_valid_water(Vector3i local_position) {
    return in_bounds(local_position) && get_block_index_at(local_position) == 0;
}

uint8_t Chunk::get_water_at(Vector3i local_position) {
    return water[position_to_index(local_position)];
}

void Chunk::set_water_at(Vector3i local_position, uint8_t water_level) {
    if (in_bounds(local_position)) {
        uint64_t index = position_to_index(local_position);

        // Do not place water over non-air blocks
        if (blocks[index] != 0) {
            return;
        }

        if (water[index] != water_level) {
            water_chunk_wake_set(local_position, true);
            modified = true;
            water_updated = true;
            water_count += water_level - (int16_t) water[index];
        }

        water[index] = water_level;

    } else {
        Vector3i global_position = local_position + get_global_position();

        if (!world->is_position_loaded(global_position)) {
            return;
        }

        Chunk* chunk = world->get_chunk_at(global_position);
        chunk->set_water_at(global_position - chunk->get_global_position(), water_level);
    }
}

void Chunk::simulate_water() {
    for (int8_t cy = 0; cy < CHUNK_SIZE_Y / WATER_CHUNK_SIZE_Y; cy++) {
    for (int8_t cz = 0; cz < CHUNK_SIZE_Z / WATER_CHUNK_SIZE_Z; cz++) {
    for (int8_t cx = 0; cx < CHUNK_SIZE_X / WATER_CHUNK_SIZE_X; cx++) {
        if (water_chunk_awake[cx + cz * CHUNK_SIZE_X + cy * CHUNK_SIZE_X * CHUNK_SIZE_Z] == 0) {
            continue;
        }

        water_chunk_awake[cx + cz * CHUNK_SIZE_X + cy * CHUNK_SIZE_X * CHUNK_SIZE_Z] = 0;

        for (uint8_t y = cy * WATER_CHUNK_SIZE_Y; y < (cy + 1) * WATER_CHUNK_SIZE_Y; y++) {
        for (uint8_t z = cz * WATER_CHUNK_SIZE_Z; z < (cz + 1) * WATER_CHUNK_SIZE_Z; z++) {
        for (uint8_t x = cx * WATER_CHUNK_SIZE_X; x < (cx + 1) * WATER_CHUNK_SIZE_X; x++) {


        }
        }
        }
    }
    }
    }
}
