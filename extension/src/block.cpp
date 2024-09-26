#include "../include/block.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void Block::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_id"), &Block::get_id);
	ClassDB::bind_method(D_METHOD("set_id", "new_id"), &Block::set_id);

    ClassDB::add_property(
        "Block",
        PropertyInfo(Variant::INT, "id"),
        "set_id",
        "get_id"
    );
}

Block::Block() {

}

Block::~Block() {

}

int64_t Block::get_id() const {
    return id;
}

void Block::set_id(int64_t new_id) {
    id = new_id;
}

void Block::simulate(int64_t state) {

}