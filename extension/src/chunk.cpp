#include "../include/chunk.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void Chunk::_bind_methods() {
    ClassDB::bind_method(D_METHOD("generate_block", "id", "position"), &Chunk::generate_block);
    ClassDB::bind_method(D_METHOD("generate_chunk"), &Chunk::generate_chunk);
    ClassDB::bind_method(D_METHOD("generate_mesh"), &Chunk::generate_mesh);
}

Chunk::Chunk() {
    vertices = PackedVector3Array();
    indices = PackedInt32Array();
    uvs = PackedVector2Array();
    normals = PackedVector3Array();

    blocks = PackedInt64Array();
    blocks.resize(CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z);
}

Chunk::~Chunk() {

}

uint64_t Chunk::get_block_id_at(Vector3 position) {
    uint64_t index = int(position.x) + int(position.z) * CHUNK_SIZE_X + int(position.y) * CHUNK_SIZE_Z * CHUNK_SIZE_X;
    return blocks[index];
}

uint64_t Chunk::position_to_index(Vector3 position) {
    return int(position.x) + int(position.z) * CHUNK_SIZE_X + int(position.y) * CHUNK_SIZE_Z * CHUNK_SIZE_X;
}

Vector3 Chunk::index_to_position(uint64_t index) {
    return Vector3(index % CHUNK_SIZE_X, index / (CHUNK_SIZE_X * CHUNK_SIZE_Z), (index / CHUNK_SIZE_X) % CHUNK_SIZE_Z);
}

bool Chunk::in_bounds(Vector3 position) {
    return
    0 <= position.x && position.x < CHUNK_SIZE_X &&
    0 <= position.y && position.y < CHUNK_SIZE_Y &&
    0 <= position.z && position.z < CHUNK_SIZE_Z;
}

void Chunk::add_face_uvs(uint64_t x, uint64_t y) {
    uvs.append(Vector2(texture_scale * x, texture_scale * y));
	uvs.append(Vector2(texture_scale * x + texture_scale, texture_scale * y));
	uvs.append(Vector2(texture_scale * x + texture_scale, texture_scale * y + texture_scale));
	uvs.append(Vector2(texture_scale * x, texture_scale * y + texture_scale));
}

void Chunk::add_face_triangles() {
    indices.append(face_count * 4 + 0);
    indices.append(face_count * 4 + 1);
    indices.append(face_count * 4 + 2);
    indices.append(face_count * 4 + 0);
    indices.append(face_count * 4 + 2);
    indices.append(face_count * 4 + 3);

    face_count++;
}

void Chunk::generate_block(uint64_t id, Vector3i position) {
    // Top
    if (!in_bounds(position + Vector3(0, 1, 0)) || get_block_id_at(position + Vector3(0, 1, 0)) == 0) {
        vertices.append(position + Vector3(0, 1, 0));
        vertices.append(position + Vector3(1, 1, 0));
        vertices.append(position + Vector3(1, 1, 1));
        vertices.append(position + Vector3(0, 1, 1));

        add_face_triangles();
        add_face_uvs(0, id);

        for (int i = 0; i < 4; i++) {
            normals.append(Vector3(0, 1, 0));
        }
    }

    // East
    if (!in_bounds(position + Vector3(1, 0, 0)) || get_block_id_at(position + Vector3(1, 0, 0)) == 0) {
        vertices.append(position + Vector3(1, 1, 1));
        vertices.append(position + Vector3(1, 1, 0));
        vertices.append(position + Vector3(1, 0, 0));
        vertices.append(position + Vector3(1, 0, 1));
        add_face_triangles();
        add_face_uvs(3, id);

        for (int i = 0; i < 4; i++) {
            normals.append(Vector3(1, 0, 0));
        }
    }

    // South
    if (!in_bounds(position + Vector3(0, 0, 1)) || get_block_id_at(position + Vector3(0, 0, 1)) == 0) {
        vertices.append(position + Vector3(0, 1, 1));
        vertices.append(position + Vector3(1, 1, 1));
        vertices.append(position + Vector3(1, 0, 1));
        vertices.append(position + Vector3(0, 0, 1));
        add_face_triangles();
        add_face_uvs(4, id);

        for (int i = 0; i < 4; i++) {
            normals.append(Vector3(0, 0, 1));
        }
    }

    // West
    if (!in_bounds(position + Vector3(-1, 0, 0)) || get_block_id_at(position + Vector3(-1, 0, 0)) == 0) {
        vertices.append(position + Vector3(0, 1, 0));
        vertices.append(position + Vector3(0, 1, 1));
        vertices.append(position + Vector3(0, 0, 1));
        vertices.append(position + Vector3(0, 0, 0));
        add_face_triangles();
        add_face_uvs(5, id);

        for (int i = 0; i < 4; i++) {
            normals.append(Vector3(-1, 0, 0));
        }
    }

    // North
    if (!in_bounds(position + Vector3(0, 0, -1)) || get_block_id_at(position + Vector3(0, 0, -1)) == 0) {
        vertices.append(position + Vector3(1, 1, 0));
        vertices.append(position + Vector3(0, 1, 0));
        vertices.append(position + Vector3(0, 0, 0));
        vertices.append(position + Vector3(1, 0, 0));
        add_face_triangles();
        add_face_uvs(2, id);

        for (int i = 0; i < 4; i++) {
            normals.append(Vector3(0, 0, -1));
        }
    }

    // Bottom
    if (!in_bounds(position + Vector3(0, -1, 0)) || get_block_id_at(position + Vector3(0, -1, 0)) == 0) {
        vertices.append(position + Vector3(0, 0, 1));
        vertices.append(position + Vector3(1, 0, 1));
        vertices.append(position + Vector3(1, 0, 0));
        vertices.append(position + Vector3(0, 0, 0));
        add_face_triangles();
        add_face_uvs(1, id);

        for (int i = 0; i < 4; i++) {
            normals.append(Vector3(0, -1, 0));
        }
    }
}

void Chunk::generate_chunk() {
    // Generate a pattern for debugging
    for (uint64_t y = 0; y < CHUNK_SIZE_Y; y++) {
        for (uint64_t z = 0; z < CHUNK_SIZE_Z; z++) {
            for (uint64_t x = 0; x < CHUNK_SIZE_X; x++) {
                if (x % 2 == 0 || z % 2 == 0 || y % 2 == 0) {
                    continue;
                }
                uint64_t idx = x + z * CHUNK_SIZE_X + y * CHUNK_SIZE_Z * CHUNK_SIZE_X;
                blocks[idx] = 1;
            }
        }
    }
    // Generate a block mesh for all blocks
    for (uint64_t y = 0; y < CHUNK_SIZE_Y; y++) {
        for (uint64_t z = 0; z < CHUNK_SIZE_Z; z++) {
            for (uint64_t x = 0; x < CHUNK_SIZE_X; x++) {
                uint64_t id = get_block_id_at(Vector3(x, y, z));
                if (id != 0) {
                    generate_block(id, Vector3i(x, y, z));
                }
            }
        }
    }
}

void Chunk::generate_mesh() {
    Array arrays;
    arrays.resize(ArrayMesh::ARRAY_MAX);

    arrays[ArrayMesh::ARRAY_VERTEX] = vertices;
    arrays[ArrayMesh::ARRAY_INDEX] = indices;
    arrays[ArrayMesh::ARRAY_TEX_UV] = uvs;
    arrays[ArrayMesh::ARRAY_NORMAL] = normals;

    Ref<ArrayMesh> array_mesh(memnew(ArrayMesh));

    array_mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, arrays);

    set_mesh(array_mesh);
}