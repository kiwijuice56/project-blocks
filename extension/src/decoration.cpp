#include "../include/decoration.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void Decoration::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_blocks"), &Decoration::get_blocks);
	ClassDB::bind_method(D_METHOD("set_blocks", "new_blocks"), &Decoration::set_blocks);
    ClassDB::bind_method(D_METHOD("get_size"), &Decoration::get_size);
	ClassDB::bind_method(D_METHOD("set_size", "new_size"), &Decoration::set_size);

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

Vector3i Decoration::get_size() const {
    return size;
}

void Decoration::set_size(Vector3i new_size) {
    size = new_size;
}
