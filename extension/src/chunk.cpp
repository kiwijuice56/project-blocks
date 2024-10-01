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
    normals = PackedVector3Array();

    blocks = PackedByteArray();
    blocks.resize(CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z);

    set_process_thread_group(PROCESS_THREAD_GROUP_SUB_THREAD);
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
    for (int i = 0; i < 6; i++) {
        normals[face_count * 6 + i] = normal;
    }
}

void Chunk::generate_data(Vector3 chunk_position) {
    block_count = 0;

    Vector2 chunk_uv = Vector2(
        Vector2i(chunk_position.x, chunk_position.z)
        / Vector2i(CHUNK_SIZE_X, CHUNK_SIZE_Z)
        % Vector2i(32, 32)
    ) / 32.0;

    Vector2 chunk_uv2 = Vector2(
        Vector2i(chunk_position.x, chunk_position.z)
        / Vector2i(CHUNK_SIZE_X, CHUNK_SIZE_Z)
        % Vector2i(16, 16)
    ) / 16.0;

    if (chunk_uv.x < 0.0) chunk_uv.x = 1.0 + chunk_uv.x;
    if (chunk_uv.y < 0.0) chunk_uv.y = 1.0 + chunk_uv.y;
    if (chunk_uv2.x < 0.0) chunk_uv2.x = 1.0 + chunk_uv2.x;
    if (chunk_uv2.y < 0.0) chunk_uv2.y = 1.0 + chunk_uv2.y;

    max_y = 0;
    for (uint64_t z = 0; z < CHUNK_SIZE_Z; z++) {
        for (uint64_t x = 0; x < CHUNK_SIZE_X; x++) {
            Vector2 uv = chunk_uv + Vector2(x, z) / Vector2(CHUNK_SIZE_X, CHUNK_SIZE_Z) / 32.0;
            double height = sample_from_noise(main_noise_texture, uv);
            uint64_t block_height = int(height * 12);
            block_height *= block_height;

            uv = chunk_uv2 + Vector2(x, z) / Vector2(CHUNK_SIZE_X, CHUNK_SIZE_Z) / 16.0;
            height = sample_from_noise(main_noise_texture, uv);
            block_height += int(height * 15);

            if (block_height >= CHUNK_SIZE_Y) {
                block_height = CHUNK_SIZE_Y - 1;
            }

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
    vertices.resize(6 * 6 * block_count);
    //uvs.resize(4 * 6 * block_count);
    normals.resize(6 * 6 * block_count);

    greedy_mesh_generation();

    // Trim off excess data
    vertices.resize(6 * face_count);
    //uvs.resize(4 * face_count);
    normals.resize(6 * face_count);

    // Package data into an ArrayMesh
    Array arrays;
    arrays.resize(ArrayMesh::ARRAY_MAX);
    arrays[ArrayMesh::ARRAY_VERTEX] = vertices;
    //arrays[ArrayMesh::ARRAY_TEX_UV] = uvs;
    arrays[ArrayMesh::ARRAY_NORMAL] = normals;

    Ref<ArrayMesh> array_mesh(memnew(ArrayMesh));
    array_mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, arrays);
    call_deferred("set_mesh", array_mesh);
    generate_static_body(true);
}

void Chunk::generate_static_body(bool force_update) {
    if (force_update || !has_static_body) {
        if (has_static_body && get_child_count() > 0) {
            Node* old_body = get_child(0);
            call_deferred("remove_child", old_body);
            old_body->call_deferred("queue_free");
        }
        Ref<ConcavePolygonShape3D> shape_data(memnew(ConcavePolygonShape3D));
        shape_data->set_faces(vertices);

        CollisionShape3D* collision_shape = memnew(CollisionShape3D);
        collision_shape->set_shape(shape_data);

        StaticBody3D* static_body = memnew(StaticBody3D);
        static_body->call_deferred("add_child", collision_shape);

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

void Chunk::greedy_mesh_generation() {
    visited = new bool[CHUNK_SIZE_X * CHUNK_SIZE_Z * max_y];
    for (uint64_t i = 0; i < CHUNK_SIZE_X * CHUNK_SIZE_Z * max_y; i++) {
        visited[i] = false;
    }

    face_count = 0;

    for (int x = 0; x < CHUNK_SIZE_X; x++) {
        for (int z = 0; z < CHUNK_SIZE_Z; z++) {
            for (int y = 0; y < max_y; y++) {
                if (greedy_invalid(Vector3(x, y, z))) {
                    continue;
                }
                Vector3 start = Vector3(x, y, z);
                Vector3 size = greedy_scan(start);
                add_rectangular_prism(start, size);
            }
        }
    }

    delete [] visited;
}

Vector3 Chunk::greedy_scan(Vector3 start) {
    Vector3 size = Vector3(1, 1, 1);
    while (!greedy_invalid(start + Vector3(size.x, 0, 0))) {
        visited[position_to_index(start + Vector3(size.x, 0, 0))] = true;
        size.x++;
    }

    while (true) {
        bool done = false;
        for (int x = 0; x < int(size.x); x++) {
            if (greedy_invalid(start + Vector3(x, 0, size.z))) {
                done = true;
                break;
            }
        }
        if (done) {
            break;
        }
        for (int x = 0; x < int(size.x); x++) {
            visited[position_to_index(start + Vector3(x, 0, size.z))] = true;
        }
        size.z++;
    }
    while (true) {
        bool done = false;
        for (int x = 0; x < int(size.x); x++) {
            for (int z = 0; z < int(size.z); z++) {
                if (greedy_invalid(start + Vector3(x, size.y, z))) {
                    done = true;
                    break;
                }
            }
        }
        if (done) {
            break;
        }
        for (int x = 0; x < int(size.x); x++) {
            for (int z = 0; z < int(size.z); z++) {
                visited[position_to_index(start + Vector3(x, size.y, z))] = true;
            }
        }
        size.y++;
    }
    return size;
}

bool Chunk::greedy_invalid(Vector3 position) {
    return !in_bounds(position) || get_block_id_at(position) == 0 || visited[position_to_index(position)];
}

void Chunk::add_rectangular_prism(Vector3 start, Vector3 size) {
    // Bottom face
    vertices[face_count * 6 + 0] = start;
    vertices[face_count * 6 + 1] = start + Vector3(0, 0, size.z);
    vertices[face_count * 6 + 2] = start + Vector3(size.x, 0, 0);
    vertices[face_count * 6 + 3] = start + Vector3(size.x, 0, 0);
    vertices[face_count * 6 + 4] = start + Vector3(0, 0, size.z);
    vertices[face_count * 6 + 5] = start + Vector3(size.x, 0, size.z);
    add_face_normals(Vector3(0, -1, 0));

    face_count++;

    // Top face
    vertices[face_count * 6 + 0] = start + Vector3(0, size.y, 0);
    vertices[face_count * 6 + 1] = start + Vector3(size.x, size.y, 0);
    vertices[face_count * 6 + 2] = start + Vector3(0, size.y, size.z);
    vertices[face_count * 6 + 3] = start + Vector3(size.x, size.y, 0);
    vertices[face_count * 6 + 4] = start + Vector3(size.x, size.y, size.z);
    vertices[face_count * 6 + 5] = start + Vector3(0, size.y, size.z);
    add_face_normals(Vector3(0, 1, 0));

    face_count++;

    // Z constant 1
    vertices[face_count * 6 + 0] = start + Vector3(0, size.y, 0);
    vertices[face_count * 6 + 1] = start;
    vertices[face_count * 6 + 2] = start + Vector3(size.x, size.y, 0);
    vertices[face_count * 6 + 3] = start + Vector3(size.x, size.y, 0);
    vertices[face_count * 6 + 4] = start;
    vertices[face_count * 6 + 5] = start + Vector3(size.x, 0, 0);
    add_face_normals(Vector3(0, 0, -1));

    face_count++;

    // Z constant 2
    vertices[face_count * 6 + 0] = start + Vector3(0, size.y, size.z);
    vertices[face_count * 6 + 1] = start + Vector3(size.x, size.y, size.z);
    vertices[face_count * 6 + 2] = start + Vector3(0, 0, size.z);
    vertices[face_count * 6 + 3] = start + Vector3(size.x, size.y, size.z);
    vertices[face_count * 6 + 4] = start + Vector3(size.x, 0, size.z);
    vertices[face_count * 6 + 5] = start + Vector3(0, 0, size.z);
    add_face_normals(Vector3(0, 0, 1));

    face_count++;

    // X constant 1
    vertices[face_count * 6 + 0] = start;
    vertices[face_count * 6 + 1] = start + Vector3(0, size.y, 0);
    vertices[face_count * 6 + 2] = start + Vector3(0, size.y, size.z);
    vertices[face_count * 6 + 3] = start;
    vertices[face_count * 6 + 4] = start + Vector3(0, size.y, size.z);
    vertices[face_count * 6 + 5] = start + Vector3(0, 0, size.z);
    add_face_normals(Vector3(-1, 0, 0));

    face_count++;

    // X constant 2
    vertices[face_count * 6 + 0] = start + Vector3(size.x, size.y, size.z);
    vertices[face_count * 6 + 1] = start + Vector3(size.x, size.y, 0);
    vertices[face_count * 6 + 2] = start + Vector3(size.x, 0, 0);

    vertices[face_count * 6 + 3] = start + Vector3(size.x, 0, 0);
    vertices[face_count * 6 + 4] = start + Vector3(size.x, 0, size.z);
    vertices[face_count * 6 + 5] = start + Vector3(size.x, size.y, size.z);
    add_face_normals(Vector3(1, 0, 0));

    face_count++;

}