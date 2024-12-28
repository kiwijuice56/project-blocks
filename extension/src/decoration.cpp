#include "../include/decoration.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void Decoration::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_blocks"), &Decoration::get_blocks);
	ClassDB::bind_method(D_METHOD("set_blocks", "new_blocks"), &Decoration::set_blocks);
    ClassDB::bind_method(D_METHOD("get_size"), &Decoration::get_size);
	ClassDB::bind_method(D_METHOD("set_size", "new_size"), &Decoration::set_size);
    ClassDB::bind_method(D_METHOD("get_internal_name"), &Decoration::get_internal_name);
	ClassDB::bind_method(D_METHOD("set_internal_name", "new_internal_name"), &Decoration::set_internal_name);

    ADD_PROPERTY(
        PropertyInfo(Variant::PACKED_INT32_ARRAY, "blocks"),
        "set_blocks",
        "get_blocks"
    );

    ADD_PROPERTY(
        PropertyInfo(Variant::VECTOR3I, "size"),
        "set_size",
        "get_size"
    );

    ADD_PROPERTY(
        PropertyInfo(Variant::STRING, "internal_name"),
        "set_internal_name",
        "get_internal_name"
    );
}

Decoration::Decoration() {

}

Decoration::~Decoration() {

}

PackedInt32Array Decoration::get_blocks() const {
    return blocks;
}

void Decoration::set_blocks(PackedInt32Array new_blocks) {
    blocks = new_blocks;
}

String Decoration::get_internal_name() const {
    return internal_name;
}

void Decoration::set_internal_name(String new_internal_name) {
    internal_name = new_internal_name;
}

Vector3i Decoration::get_size() const {
    return size;
}

void Decoration::set_size(Vector3i new_size) {
    size = new_size;
}
