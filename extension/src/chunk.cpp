#include "../include/world.h"
#include "../include/chunk.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void Chunk::_bind_methods() {

}

Chunk::Chunk() {
    visited = new bool[CHUNK_SIZE_X * CHUNK_SIZE_Z * CHUNK_SIZE_Y];

    blocks.resize(CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z);

    water.resize(CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z);
    water_buffer.resize(CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z);

    // Initialize StaticBody3D data
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

// Uses local position
uint64_t Chunk::get_block_index_at(Vector3i position) {
    return blocks[uint64_t(position.x) + uint64_t(position.z) * CHUNK_SIZE_X + uint64_t(position.y) * CHUNK_SIZE_Z * CHUNK_SIZE_X];
}

uint64_t Chunk::get_block_index_at_global(Vector3i g_position) {
    g_position -= Vector3i(get_global_position());
    return blocks[uint64_t(g_position.x) + uint64_t(g_position.z) * CHUNK_SIZE_X + uint64_t(g_position.y) * CHUNK_SIZE_Z * CHUNK_SIZE_X];
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

// Keep track of block count and other state
void Chunk::calculate_block_statistics() {
    uint8_t main_block_type = blocks[0];
    uniform = true;
    has_water = false;
    block_count = 0;
    for (uint64_t i = 0; i < blocks.size(); i++) {
        uniform = blocks[i] == main_block_type;
        if (blocks[i] > 0) {
            block_count++;
        }
    }
    for (uint64_t i = 0; i < water.size(); i++) {
        if (water[i] > 0) {
            has_water = true;
        }
    }
}

void Chunk::generate_mesh(bool immediate) {
    if (block_count == 0 && !has_water) {
        clear_collision();
        call_deferred("set_visible", false);
        return;
    }

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

    Ref<ArrayMesh> array_mesh(memnew(ArrayMesh));

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

    // Third pass (water)

    vertices.clear();
    normals.clear();
    uvs.clear();
    uvs2.clear();

    greedy_mesh_generation(false, true);

    arrays[ArrayMesh::ARRAY_VERTEX] = vertices;
    arrays[ArrayMesh::ARRAY_NORMAL] = normals;
    arrays[ArrayMesh::ARRAY_TEX_UV] = uvs;
    arrays[ArrayMesh::ARRAY_TEX_UV2] = uvs2;

    if (vertices.size() > 0) {
        array_mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, arrays);
        array_mesh->surface_set_material(material_idx, water_material);
        material_idx += 1;
    }

    call_deferred("set_mesh", array_mesh);
    call_deferred("set_visible", true);
}

void Chunk::clear_collision() {
    shape_data_opaque->call_deferred("set_faces", PackedVector3Array());
    shape_data_transparent->call_deferred("set_faces", PackedVector3Array());
}

// Uses global position
void Chunk::remove_block_at(Vector3i global_position) {
    Vector3i block_position = global_position - Vector3i(get_global_position());
    uint64_t index = position_to_index(block_position);

    if (blocks[index] == 0) {
        return;
    }

    blocks[index] = 0;
    block_count--;
    modified = true;

    generate_mesh(true);
}

// Uses global position
void Chunk::place_block_at(Vector3i global_position, uint32_t block_index) {
    Vector3i block_position = global_position - Vector3i(get_global_position());
    uint64_t index = position_to_index(block_position);

    if (blocks[index] != 0) {
        return;
    }

    blocks[index] = block_index;
    block_count++;
    modified = true;

    generate_mesh(true);
}

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
                    size = Vector3(1, water[position_to_index(Vector3i(x, y, z))] / 255., 1);
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

        return false;
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

Chunk::WaterQuery Chunk::get_water_at(Vector3i local_position) {
    Chunk::WaterQuery result;
    if (in_bounds(local_position) && get_block_index_at(local_position) == 0) {
        result.water = water[position_to_index(local_position)];
        result.valid = true;
    } else {
        result.valid = false;
    }
    return result;
}

void Chunk::set_water_at(Vector3i global_position, uint8_t water_level) {
    Vector3i block_position = global_position - Vector3i(get_global_position());
    uint64_t index = position_to_index(block_position);

    if (blocks[index] != 0) {
        return;
    }

    water[index] = water_level;
    generate_mesh(false);

    modified = true;
    if (water_level > 0) {
        has_water = true;
    }
}

void Chunk::simulate_water() {
    // Translation step
    for (uint8_t y = 0; y < CHUNK_SIZE_Y; y++) {
        for (uint8_t z = 0; z < CHUNK_SIZE_Z; z++) {
            for (uint8_t x = 0; x < CHUNK_SIZE_X; x++) {
                if (get_block_index_at(Vector3i(x, y, z)) != 0) {
                    continue;
                }

                Chunk::WaterQuery q = get_water_at(Vector3i(x, y + 1, z));
                if (!q.valid) {
                    continue;
                }
                uint8_t w_u = q.water;
                if (w_u == 0) {
                    continue;
                }

                uint8_t w_0 = water[position_to_index(Vector3i(x, y, z))];
                uint16_t w_0ns = (uint16_t) (w_0 + (w_u > 1 ? w_u * 0.9 : 1));
                if (w_0ns > 255) {
                    w_0ns = 255;
                }
                uint8_t w_0n = (uint8_t) w_0ns;

                water[position_to_index(Vector3i(x, y, z))] = w_0n;
                water[position_to_index(Vector3i(x, y + 1, z))] = w_u - (w_0n - w_0);
            }
        }
    }

    // Diffusion step
    for (uint8_t y = 0; y < CHUNK_SIZE_Y; y++) {
        for (uint8_t z = 0; z < CHUNK_SIZE_Z; z++) {
            for (uint8_t x = 0; x < CHUNK_SIZE_X; x++) {
                if (get_block_index_at(Vector3i(x, y, z)) != 0) {
                    water_buffer[position_to_index(Vector3i(x, y, z))] = 0;
                    continue;
                }

                uint8_t w_0 = water[position_to_index(Vector3i(x, y, z))];

                float total_water = w_0;
                uint8_t count = 1;

                Chunk::WaterQuery w_1 = get_water_at(Vector3i(x - 1, y, z + 0));
                Chunk::WaterQuery w_2 = get_water_at(Vector3i(x + 1, y, z + 0));
                Chunk::WaterQuery w_3 = get_water_at(Vector3i(x + 0, y, z + 1));
                Chunk::WaterQuery w_4 = get_water_at(Vector3i(x + 0, y, z - 1));

                if (w_1.valid) {
                    total_water += w_1.water;
                    count++;
                }
                if (w_2.valid) {
                    total_water += w_2.water;
                    count++;
                }
                if (w_3.valid) {
                    total_water += w_3.water;
                    count++;
                }
                if (w_4.valid) {
                    total_water += w_4.water;
                    count++;
                }

                water_buffer[position_to_index(Vector3i(x, y, z))] = (uint8_t) UtilityFunctions::round(total_water / count);
            }
        }
    }
}

void Chunk::copy_water_buffer() {
    has_water = false;
    for (uint64_t i = 0; i < CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z; i++) {
        water[i] = water_buffer[i];
        if (water[i] != 0) {
            has_water = true;
        }
    }
}