#include "../include/item_state.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void ItemState::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_id"), &ItemState::get_id);
	ClassDB::bind_method(D_METHOD("set_id", "new_id"), &ItemState::set_id);
    ClassDB::bind_method(D_METHOD("get_count"), &ItemState::get_count);
	ClassDB::bind_method(D_METHOD("set_count", "new_count"), &ItemState::set_count);

    ADD_PROPERTY(
        PropertyInfo(Variant::INT, "id"),
        "set_id",
        "get_id"
    );

    ADD_PROPERTY(
        PropertyInfo(Variant::INT, "count"),
        "set_count",
        "get_count"
    );
}

ItemState::ItemState() {

}

ItemState::~ItemState() {

}

uint32_t ItemState::get_id() const {
    return id;
}

void ItemState::set_id(uint32_t new_id) {
    id = new_id;
}

uint32_t ItemState::get_count() const {
    return count;
}

void ItemState::set_count(uint32_t new_count) {
    count = new_count;
}

