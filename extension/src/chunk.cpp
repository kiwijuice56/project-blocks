#include "../include/chunk.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void Chunk::_bind_methods() {
    ClassDB::bind_method(D_METHOD("generate_data"), &Chunk::generate_data);
    ClassDB::bind_method(D_METHOD("generate_mesh"), &Chunk::generate_mesh);

    ClassDB::bind_method(D_METHOD("get_main_noise_texture"), &Chunk::get_main_noise_texture);
	ClassDB::bind_method(D_METHOD("set_main_noise_texture", "new_texture"), &Chunk::set_main_noise_texture);

    ClassDB::bind_method(D_METHOD("remove_block_at"), &Chunk::remove_block_at);

    ClassDB::add_property(
        "Chunk",
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

Chunk::Chunk() {
    vertices = PackedVector3Array();
    uvs = PackedVector2Array();
    uvs2 = PackedVector2Array();
    normals = PackedVector3Array();

    visited = new bool[CHUNK_SIZE_X * CHUNK_SIZE_Z * CHUNK_SIZE_Y];

    blocks = PackedByteArray();
    blocks.resize(CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z);

    // Initialize StaticBody3D data
    shape_data = memnew(ConcavePolygonShape3D);

    CollisionShape3D* collision_shape = memnew(CollisionShape3D);
    collision_shape->set_shape(shape_data);

    StaticBody3D* static_body = memnew(StaticBody3D);
    static_body->call_deferred("add_child", collision_shape);

    add_child(static_body);
}

Chunk::~Chunk() {
    delete [] visited;
}

Ref<Material> Chunk::get_block_material() const {
    return block_material;
}

void Chunk::set_block_material(Ref<Material> new_material) {
    block_material = new_material;
    set_material_override(block_material);
}

Ref<NoiseTexture2D> Chunk::get_main_noise_texture() const {
    return main_noise_texture;
}

void Chunk::set_main_noise_texture(Ref<NoiseTexture2D> new_texture) {
    main_noise_texture = new_texture;
}

uint64_t Chunk::get_block_id_at(Vector3i position) {
    return blocks[uint64_t(position.x) + uint64_t(position.z) * CHUNK_SIZE_X + uint64_t(position.y) * CHUNK_SIZE_Z * CHUNK_SIZE_X];
}

uint64_t Chunk::position_to_index(Vector3i position) {
    return uint64_t(position.x) + uint64_t(position.z) * CHUNK_SIZE_X + uint64_t(position.y) * CHUNK_SIZE_Z * CHUNK_SIZE_X;
}

Vector3i Chunk::index_to_position(uint64_t index) {
    return Vector3i(index % CHUNK_SIZE_X, index / (CHUNK_SIZE_X * CHUNK_SIZE_Z), (index / CHUNK_SIZE_X) % CHUNK_SIZE_Z);
}

double Chunk::sample_from_noise(Ref<NoiseTexture2D> texture, Vector2 uv) {
    Ref<Image> img = texture->get_image();
    return img->get_pixel(uint64_t(uv.x * (img->get_width() - 1)), uint64_t(uv.y * (img->get_height() - 1))).r;
}

bool Chunk::in_bounds(Vector3i position) {
    return
    0 <= position.x && position.x < CHUNK_SIZE_X &&
    0 <= position.y && position.y < CHUNK_SIZE_Y &&
    0 <= position.z && position.z < CHUNK_SIZE_Z;
}

// Generate the block data for this chunk
void Chunk::generate_data(Vector3i chunk_position) {
    // Basic "mountains" biome for testing

    blocks.fill(0);
    block_count = 0;

    Vector2 chunk_uv = Vector2(
        Vector2i(chunk_position.x, chunk_position.z)
        / Vector2i(CHUNK_SIZE_X, CHUNK_SIZE_Z)
        % Vector2i(32, 32)
    ) / 32.0;

    if (chunk_uv.x < 0.0) chunk_uv.x = 1.0 + chunk_uv.x;
    if (chunk_uv.y < 0.0) chunk_uv.y = 1.0 + chunk_uv.y;

    for (int64_t z = 0; z < CHUNK_SIZE_Z; z++) {
        for (int64_t x = 0; x < CHUNK_SIZE_X; x++) {
            Vector2 uv = chunk_uv + Vector2(x, z) / Vector2(CHUNK_SIZE_X, CHUNK_SIZE_Z) / 32.0;

            double height = sample_from_noise(main_noise_texture, uv);
            int64_t block_height = 1 + int(height * 64);
            for (int64_t y = 0; y < CHUNK_SIZE_Y; y++) {
                int64_t real_height = y + chunk_position.y;
                uint64_t block_type = 0;

                if (block_height == real_height) {
                    block_type = 3;
                } else if (0 < block_height - real_height && block_height - real_height < 5 ) {
                    block_type = 2;
                } else if (real_height < block_height) {
                    block_type = 1;
                }

                blocks[position_to_index(Vector3i(x, y, z))] = block_type;

                if (block_type > 0) {
                    block_count++;
                }
            }
        }
    }

    uniform = block_count == uint64_t(CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z);
}

void Chunk::generate_mesh() {
    if (block_count == 0) {
        clear_collision();
        call_deferred("set_visible", false);
        return;
    }

    vertices.clear();
    normals.clear();
    uvs.clear();
    uvs2.clear();

    greedy_mesh_generation();

    // Package data into an ArrayMesh
    Array arrays;
    arrays.resize(ArrayMesh::ARRAY_MAX);
    arrays[ArrayMesh::ARRAY_VERTEX] = vertices;
    arrays[ArrayMesh::ARRAY_NORMAL] = normals;
    arrays[ArrayMesh::ARRAY_TEX_UV] = uvs;
    arrays[ArrayMesh::ARRAY_TEX_UV2] = uvs2;

    Ref<ArrayMesh> array_mesh(memnew(ArrayMesh));

    array_mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, arrays);

    call_deferred("set_mesh", array_mesh);
    call_deferred("set_visible", true);

    shape_data->call_deferred("set_faces", vertices);
}

void Chunk::clear_collision() {
    shape_data->call_deferred("set_faces", PackedVector3Array());
}

void Chunk::remove_block_at(Vector3i global_position) {
    Vector3i block_position = global_position - Vector3i(get_global_position());
    uint64_t index = position_to_index(block_position);

    if (blocks[index] > 0)
        block_count--;

    blocks[index] = 0;
    generate_mesh();
}

// Fill vertex, normal, and uv arrays with proper triangles (using the greedy meshing algorithm)
void Chunk::greedy_mesh_generation() {
    for (uint64_t i = 0; i < CHUNK_SIZE_X * CHUNK_SIZE_Z * CHUNK_SIZE_Y; i++) {
        visited[i] = false;
    }

    face_count = 0;

    for (uint64_t x = 0; x < CHUNK_SIZE_X; x++) {
        for (uint64_t z = 0; z < CHUNK_SIZE_Z; z++) {
            for (uint64_t y = 0; y < CHUNK_SIZE_Y; y++) {
                current_greedy_block = get_block_id_at(Vector3i(x, y, z));
                if (greedy_invalid(Vector3i(x, y, z)))
                    continue;
                Vector3i start = Vector3i(x, y, z);
                Vector3i size = greedy_scan(start);
                add_rectangular_prism(start, size);
            }
        }
    }
}

// Greedily find the size of the largest prism we can add to our mesh
Vector3i Chunk::greedy_scan(Vector3i start) {
    Vector3i size = Vector3i(1, 1, 1);
    while (!greedy_invalid(start + Vector3i(size.x, 0, 0))) {
        visited[position_to_index(start + Vector3i(size.x, 0, 0))] = true;
        size.x++;
    }

    bool axis_done = false;
    while (!axis_done) {
        for (uint8_t x = 0; x < uint8_t(size.x); x++) {
            if (greedy_invalid(start + Vector3i(x, 0, size.z))) {
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
                if (greedy_invalid(start + Vector3i(x, size.y, z))) {
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
bool Chunk::greedy_invalid(Vector3i position) {
    if (!in_bounds(position) || visited[position_to_index(position)]) {
        return true;
    }

    uint64_t block_id = get_block_id_at(position);

    if (block_id == 0) {
        return true;
    }

    if (block_id == current_greedy_block) {
        return false;
    }

    bool fully_covered =
        in_bounds(Vector3i(+1, 0, 0) + position) && get_block_id_at(Vector3i(+1, 0, 0) + position) != 0 &&
        in_bounds(Vector3i(-1, 0, 0) + position) && get_block_id_at(Vector3i(-1, 0, 0) + position) != 0 &&
        in_bounds(Vector3i(0, +1, 0) + position) && get_block_id_at(Vector3i(0, +1, 0) + position) != 0 &&
        in_bounds(Vector3i(0, -1, 0) + position) && get_block_id_at(Vector3i(0, -1, 0) + position) != 0 &&
        in_bounds(Vector3i(0, 0, +1) + position) && get_block_id_at(Vector3i(0, 0, +1) + position) != 0 &&
        in_bounds(Vector3i(0, 0, -1) + position) && get_block_id_at(Vector3i(0, 0, -1) + position) != 0;

    return !fully_covered;
}

// Adds vertices, uvs, and normals for a rectangular prism to our mesh
void Chunk::add_rectangular_prism(Vector3i start, Vector3i size) {
    vertices.resize(vertices.size() + 36);
    normals.resize(normals.size() + 36);
    uvs.resize(uvs.size() + 36);
    uvs2.resize(uvs2.size() + 36);

    // Y, facing up
    vertices[face_count * 6 + 0] = start + Vector3i(0, size.y, size.z);
    vertices[face_count * 6 + 1] = start + Vector3i(0, size.y, 0);
    vertices[face_count * 6 + 2] = start + Vector3i(size.x, size.y, 0);
    vertices[face_count * 6 + 3] = start + Vector3i(0, size.y, size.z);
    vertices[face_count * 6 + 4] = start + Vector3i(size.x, size.y, 0);
    vertices[face_count * 6 + 5] = start + Vector3i(size.x, size.y, size.z);
    add_face_normals(Vector3i(0, 1, 0));
    add_face_uvs(current_greedy_block * 6, Vector2i(size.x, size.z));

    face_count++;

    // Y, facing down
    vertices[face_count * 6 + 0] = start + Vector3i(0, 0, 0);
    vertices[face_count * 6 + 1] = start + Vector3i(0, 0, size.z);
    vertices[face_count * 6 + 2] = start + Vector3i(size.x, 0, size.z);
    vertices[face_count * 6 + 3] = start + Vector3i(0, 0, 0);
    vertices[face_count * 6 + 4] = start + Vector3i(size.x, 0, size.z);
    vertices[face_count * 6 + 5] = start + Vector3i(size.x, 0, 0);
    add_face_normals(Vector3i(0, -1, 0));
    add_face_uvs(current_greedy_block * 6 + 1, Vector2i(size.x, size.z));

    face_count++;

    // Z, facing back
    vertices[face_count * 6 + 0] = start + Vector3i(size.x, 0, 0);
    vertices[face_count * 6 + 1] = start + Vector3i(size.x, size.y, 0);
    vertices[face_count * 6 + 2] = start + Vector3i(0, size.y, 0);
    vertices[face_count * 6 + 3] = start + Vector3i(size.x, 0, 0);
    vertices[face_count * 6 + 4] = start + Vector3i(0, size.y, 0);
    vertices[face_count * 6 + 5] = start + Vector3i(0, 0, 0);
    add_face_normals(Vector3i(0, 0, -1));
    add_face_uvs(current_greedy_block * 6 + 2, Vector2i(size.x, size.y));

    face_count++;

    // Z, facing forward
    vertices[face_count * 6 + 0] = start + Vector3i(0, 0, size.z);
    vertices[face_count * 6 + 1] = start + Vector3i(0, size.y, size.z);
    vertices[face_count * 6 + 2] = start + Vector3i(size.x, size.y, size.z);
    vertices[face_count * 6 + 3] = start + Vector3i(0, 0, size.z);
    vertices[face_count * 6 + 4] = start + Vector3i(size.x, size.y, size.z);
    vertices[face_count * 6 + 5] = start + Vector3i(size.x, 0, size.z);
    add_face_normals(Vector3i(0, 0, 1));
    add_face_uvs(current_greedy_block * 6 + 3, Vector2i(size.x, size.y));

    face_count++;

    // X, facing left
    vertices[face_count * 6 + 0] = start + Vector3i(0, 0, 0);
    vertices[face_count * 6 + 1] = start + Vector3i(0, size.y, 0);
    vertices[face_count * 6 + 2] = start + Vector3i(0, size.y, size.z);
    vertices[face_count * 6 + 3] = start + Vector3i(0, 0, 0);
    vertices[face_count * 6 + 4] = start + Vector3i(0, size.y, size.z);
    vertices[face_count * 6 + 5] = start + Vector3i(0, 0, size.z);
    add_face_normals(Vector3i(-1, 0, 0));
    add_face_uvs(current_greedy_block * 6 + 4, Vector2i(size.z, size.y));

    face_count++;

    // X, facing right
    vertices[face_count * 6 + 0] = start + Vector3i(size.x, 0, size.z);
    vertices[face_count * 6 + 1] = start + Vector3i(size.x, size.y, size.z);
    vertices[face_count * 6 + 2] = start + Vector3i(size.x, size.y, 0);
    vertices[face_count * 6 + 3] = start + Vector3i(size.x, 0, size.z);
    vertices[face_count * 6 + 4] = start + Vector3i(size.x, size.y, 0);
    vertices[face_count * 6 + 5] = start + Vector3i(size.x, 0, 0);
    add_face_normals(Vector3i(1, 0, 0));
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