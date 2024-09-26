#include "../include/chunk.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void Chunk::_bind_methods() {
    ClassDB::bind_method(D_METHOD("generate_cube", "position"), &Chunk::generate_cube);
    ClassDB::bind_method(D_METHOD("generate_chunk"), &Chunk::generate_chunk);
    ClassDB::bind_method(D_METHOD("generate_mesh"), &Chunk::generate_mesh);
}

Chunk::Chunk() {
    vertices = PackedVector3Array();
    indices = PackedInt32Array();
    uvs = PackedVector2Array();

    blocks = PackedInt64Array();
    blocks.resize(CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z);
}

Chunk::~Chunk() {

}

int64_t Chunk::get_block_id_at(Vector3 position) {
    int64_t index = int(position.x) + int(position.z) * CHUNK_SIZE_X + int(position.y) * CHUNK_SIZE_Z * CHUNK_SIZE_X;
    return blocks[index];
}

void Chunk::add_face_uvs(int64_t x, int64_t y) {
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

void Chunk::generate_cube(Vector3 position) {
    // Top
    vertices.append(position + Vector3(-0.5,  0.5, -0.5));
    vertices.append(position + Vector3( 0.5,  0.5, -0.5));
    vertices.append(position + Vector3( 0.5,  0.5,  0.5));
    vertices.append(position + Vector3(-0.5,  0.5,  0.5));
    add_face_triangles();
    add_face_uvs(0, 0);

    // East
    vertices.append(position + Vector3( 0.5,  0.5,  0.5));
    vertices.append(position + Vector3( 0.5,  0.5, -0.5));
    vertices.append(position + Vector3( 0.5, -0.5, -0.5));
    vertices.append(position + Vector3( 0.5, -0.5,  0.5));
    add_face_triangles();
    add_face_uvs(3, 0);

    // South
    vertices.append(position + Vector3(-0.5,  0.5,  0.5));
    vertices.append(position + Vector3( 0.5,  0.5,  0.5));
    vertices.append(position + Vector3( 0.5, -0.5,  0.5));
    vertices.append(position + Vector3(-0.5, -0.5,  0.5));
    add_face_triangles();
    add_face_uvs(0, 1);

    // West
    vertices.append(position + Vector3(-0.5,  0.5, -0.5));
    vertices.append(position + Vector3(-0.5,  0.5,  0.5));
    vertices.append(position + Vector3(-0.5, -0.5,  0.5));
    vertices.append(position + Vector3(-0.5, -0.5, -0.5));
    add_face_triangles();
    add_face_uvs(1, 1);

    // North
    vertices.append(position + Vector3( 0.5,  0.5, -0.5));
    vertices.append(position + Vector3(-0.5,  0.5, -0.5));
    vertices.append(position + Vector3(-0.5, -0.5, -0.5));
    vertices.append(position + Vector3( 0.5, -0.5, -0.5));
    add_face_triangles();
    add_face_uvs(2, 0);

    // Bottom
    vertices.append(position + Vector3(-0.5, -0.5,  0.5));
    vertices.append(position + Vector3( 0.5, -0.5,  0.5));
    vertices.append(position + Vector3( 0.5, -0.5, -0.5));
    vertices.append(position + Vector3(-0.5, -0.5, -0.5));
    add_face_triangles();
    add_face_uvs(1, 0);
}

void Chunk::generate_chunk() {
    for (int i = 0; i < blocks.size(); i++) {
        if (i % 2 == 0) {
            blocks[i] = 1;
        } else {
            blocks[i] = 0;
        }
    }
    for (int i = 0; i < CHUNK_SIZE_X; i++) {
        for (int j = 0; j < CHUNK_SIZE_Z; j++) {
            for (int k = 0; k < CHUNK_SIZE_Y; k++) {
                if (get_block_id_at(Vector3(i, k, j)) != 0) {
                    generate_cube(Vector3(i, k, j));
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

    Ref<ArrayMesh> array_mesh(memnew(ArrayMesh));

    array_mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, arrays);

    set_mesh(array_mesh);
}