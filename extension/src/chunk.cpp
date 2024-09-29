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
    indices = PackedInt32Array();
    uvs = PackedVector2Array();
    normals = PackedVector3Array();
    collision_vertices = PackedVector3Array();

    blocks = PackedByteArray();
    blocks.resize(CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z);
}

Chunk::~Chunk() {

}

uint64_t Chunk::get_id() const {
    return id;
}

void Chunk::set_id(uint64_t new_id) {
    id = new_id;
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

double Chunk::sample_from_noise(Ref<NoiseTexture2D> texture, Vector2 uv) {
    Ref<Image> img = texture->get_image();
    return img->get_pixel(int(uv.x * (img->get_width() - 1)), int(uv.y * (img->get_height() - 1))).r;
}

bool Chunk::in_bounds(Vector3 position) {
    return
    0 <= position.x && position.x < CHUNK_SIZE_X &&
    0 <= position.y && position.y < CHUNK_SIZE_Y &&
    0 <= position.z && position.z < CHUNK_SIZE_Z;
}

void Chunk::add_face_uvs(uint64_t x, uint64_t y) {
    uvs[face_count * 4 + 0] = Vector2(texture_scale * x, texture_scale * y);
	uvs[face_count * 4 + 1] = Vector2(texture_scale * x + texture_scale, texture_scale * y);
	uvs[face_count * 4 + 2] = Vector2(texture_scale * x + texture_scale, texture_scale * y + texture_scale);
	uvs[face_count * 4 + 3] = Vector2(texture_scale * x, texture_scale * y + texture_scale);
}

void Chunk::add_face_normals(Vector3 normal) {
    for (int i = 0; i < 4; i++) {
        normals[face_count * 4 + i] = normal;
    }
}

void Chunk::add_face_triangles() {
    indices[face_count * 6 + 0] = face_count * 4 + 0;
    indices[face_count * 6 + 1] = face_count * 4 + 1;
    indices[face_count * 6 + 2] = face_count * 4 + 2;
    indices[face_count * 6 + 3] = face_count * 4 + 0;
    indices[face_count * 6 + 4] = face_count * 4 + 2;
    indices[face_count * 6 + 5] = face_count * 4 + 3;

    for (uint64_t i = 0; i < 6; i++) {
        collision_vertices[face_count * 6 + i] = vertices[indices[face_count * 6 + i]];
    }
}

void Chunk::generate_block_faces(uint64_t id, Vector3i position) {
    // Top
    if (!in_bounds(position + Vector3(0, 1, 0)) || get_block_id_at(position + Vector3(0, 1, 0)) == 0) {
        vertices[face_count * 4 + 0] = position + Vector3(0, 1, 0);
        vertices[face_count * 4 + 1] = position + Vector3(1, 1, 0);
        vertices[face_count * 4 + 2] = position + Vector3(1, 1, 1);
        vertices[face_count * 4 + 3] = position + Vector3(0, 1, 1);
        add_face_uvs(0, id);
        add_face_triangles();
        add_face_normals(Vector3(0, 1, 0));
        face_count++;
    }

    // East
    if (!in_bounds(position + Vector3(1, 0, 0)) || get_block_id_at(position + Vector3(1, 0, 0)) == 0) {
        vertices[face_count * 4 + 0] = position + Vector3(1, 1, 1);
        vertices[face_count * 4 + 1] = position + Vector3(1, 1, 0);
        vertices[face_count * 4 + 2] = position + Vector3(1, 0, 0);
        vertices[face_count * 4 + 3] = position + Vector3(1, 0, 1);
        add_face_triangles();
        add_face_uvs(3, id);
        add_face_normals(Vector3(1, 0, 0));
        face_count++;
    }

    // South
    if (!in_bounds(position + Vector3(0, 0, 1)) || get_block_id_at(position + Vector3(0, 0, 1)) == 0) {
        vertices[face_count * 4 + 0] = position + Vector3(0, 1, 1);
        vertices[face_count * 4 + 1] = position + Vector3(1, 1, 1);
        vertices[face_count * 4 + 2] = position + Vector3(1, 0, 1);
        vertices[face_count * 4 + 3] = position + Vector3(0, 0, 1);
        add_face_triangles();
        add_face_uvs(4, id);
        add_face_normals(Vector3(0, 0, 1));
        face_count++;
    }

    // West
    if (!in_bounds(position + Vector3(-1, 0, 0)) || get_block_id_at(position + Vector3(-1, 0, 0)) == 0) {
        vertices[face_count * 4 + 0] = position + Vector3(0, 1, 0);
        vertices[face_count * 4 + 1] = position + Vector3(0, 1, 1);
        vertices[face_count * 4 + 2] = position + Vector3(0, 0, 1);
        vertices[face_count * 4 + 3] = position + Vector3(0, 0, 0);
        add_face_triangles();
        add_face_uvs(5, id);
        add_face_normals(Vector3(-1, 0, 0));
        face_count++;
    }

    // North
    if (!in_bounds(position + Vector3(0, 0, -1)) || get_block_id_at(position + Vector3(0, 0, -1)) == 0) {
        vertices[face_count * 4 + 0] = position + Vector3(1, 1, 0);
        vertices[face_count * 4 + 1] = position + Vector3(0, 1, 0);
        vertices[face_count * 4 + 2] = position + Vector3(0, 0, 0);
        vertices[face_count * 4 + 3] = position + Vector3(1, 0, 0);
        add_face_triangles();
        add_face_uvs(2, id);
        add_face_normals(Vector3(0, 0, -1));
        face_count++;
    }

    // Bottom
    if (!in_bounds(position + Vector3(0, -1, 0)) || get_block_id_at(position + Vector3(0, -1, 0)) == 0) {
        vertices[face_count * 4 + 0] = position + Vector3(0, 0, 1);
        vertices[face_count * 4 + 1] = position + Vector3(1, 0, 1);
        vertices[face_count * 4 + 2] = position + Vector3(1, 0, 0);
        vertices[face_count * 4 + 3] = position + Vector3(0, 0, 0);
        add_face_triangles();
        add_face_uvs(1, id);
        add_face_normals(Vector3(0, -1, 0));
        face_count++;
    }
}

void Chunk::generate_data() {
    block_count = 0;

    Vector2 chunk_uv = Vector2(
        Vector2i(get_position().x, get_position().z)
        / Vector2i(CHUNK_SIZE_X, CHUNK_SIZE_Z)
        % Vector2i(32, 32)
    ) / 32.0;

    if (chunk_uv.x < 0.0) {
        chunk_uv.x = 1.0 + chunk_uv.x;
    }

    if (chunk_uv.y < 0.0) {
        chunk_uv.y = 1.0 + chunk_uv.y;
    }

    max_y = 0;
    for (uint64_t z = 0; z < CHUNK_SIZE_Z; z++) {
        for (uint64_t x = 0; x < CHUNK_SIZE_X; x++) {
            Vector2 uv = chunk_uv + Vector2(x, z) / Vector2(CHUNK_SIZE_X, CHUNK_SIZE_Z) / 32.0;
            double height = sample_from_noise(main_noise_texture, uv);
            uint64_t block_height = 1 + int(height * 24);
            max_y = std::max(max_y, block_height);

            for (uint64_t y = 0; y < block_height; y++) {
                blocks[position_to_index(Vector3(x, y, z))] = 1;
                block_count++;
            }
        }
    }
}

void Chunk::generate_mesh() {
    // Resize mesh data arrays to upper bounds of their sizes before culling
    // Drastically improves performance due to not needing to resize arrays constantly

    vertices.resize(4 * 6 * block_count);
    indices.resize(6 * 6 * block_count);
    uvs.resize(4 * 6 * block_count);
    normals.resize(4 * 6 * block_count);
    collision_vertices.resize(6 * 6 * block_count);

    face_count = 0;

    // Generate a block mesh for all blocks
    for (uint64_t y = 0; y <= max_y; y++) {
        for (uint64_t z = 0; z < CHUNK_SIZE_Z; z++) {
            for (uint64_t x = 0; x < CHUNK_SIZE_X; x++) {
                uint64_t id = get_block_id_at(Vector3(x, y, z));
                if (id != 0) {
                    generate_block_faces(id, Vector3i(x, y, z));
                }
            }
        }
    }

    // Trim off excess data
    vertices.resize(4 * face_count);
    indices.resize(6 * face_count);
    uvs.resize(4 * face_count);
    normals.resize(4 * face_count);
    collision_vertices.resize(6 * face_count);

    // Package data into an ArrayMesh
    Array arrays;
    arrays.resize(ArrayMesh::ARRAY_MAX);
    arrays[ArrayMesh::ARRAY_VERTEX] = vertices;
    arrays[ArrayMesh::ARRAY_INDEX] = indices;
    arrays[ArrayMesh::ARRAY_TEX_UV] = uvs;
    arrays[ArrayMesh::ARRAY_NORMAL] = normals;

    ArrayMesh* array_mesh(memnew(ArrayMesh));
    array_mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, arrays);
    set_mesh(array_mesh);
    generate_static_body(true);
}

void Chunk::generate_static_body(bool force_update) {
    if (force_update || !has_static_body) {
        if (has_static_body && get_child_count() > 0) {
            Node* old_body = get_child(0);
            remove_child(old_body);
            old_body->queue_free();
        }
        Ref<ConcavePolygonShape3D> shape_data(memnew(ConcavePolygonShape3D));
        shape_data->set_faces(collision_vertices);

        CollisionShape3D* collision_shape = memnew(CollisionShape3D);
        collision_shape->set_shape(shape_data);

        StaticBody3D* static_body = memnew(StaticBody3D);
        static_body->add_child(collision_shape);

        call_deferred("add_child", static_body);
        has_static_body = true;
    }
}

void Chunk::remove_block_at(Vector3 global_position) {
    Vector3 block_position = Vector3i(global_position - get_global_position());
    uint64_t index = position_to_index(block_position);
    if (blocks[index] > 0) {
        block_count--;
    }
    blocks[index] = 0;
    generate_mesh();
}