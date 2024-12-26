#include "../include/item.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void Item::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_id"), &Item::get_id);
	ClassDB::bind_method(D_METHOD("set_id", "new_id"), &Item::set_id);
    ClassDB::bind_method(D_METHOD("get_count"), &Item::get_count);
	ClassDB::bind_method(D_METHOD("set_count", "new_count"), &Item::set_count);
    ClassDB::bind_method(D_METHOD("get_stack_size"), &Item::get_stack_size);
	ClassDB::bind_method(D_METHOD("set_stack_size", "new_stack_size"), &Item::set_stack_size);
    ClassDB::bind_method(D_METHOD("get_can_drop"), &Item::get_can_drop);
	ClassDB::bind_method(D_METHOD("set_can_drop", "new_val"), &Item::set_can_drop);
    ClassDB::bind_method(D_METHOD("get_display_name"), &Item::get_display_name);
	ClassDB::bind_method(D_METHOD("set_display_name", "new_display_name"), &Item::set_display_name);
    ClassDB::bind_method(D_METHOD("get_icon"), &Item::get_icon);
	ClassDB::bind_method(D_METHOD("set_icon", "new_icon"), &Item::set_icon);

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

    ADD_PROPERTY(
        PropertyInfo(Variant::INT, "stack_size"),
        "set_stack_size",
        "get_stack_size"
    );

    ADD_PROPERTY(
        PropertyInfo(Variant::BOOL, "can_drop"),
        "set_can_drop",
        "get_can_drop"
    );

    ADD_PROPERTY(
        PropertyInfo(Variant::STRING, "display_name"),
        "set_display_name",
        "get_display_name"
    );

    ADD_PROPERTY(
        PropertyInfo(
            Variant::OBJECT,
            "icon",
            PROPERTY_HINT_RESOURCE_TYPE,
            "Texture2D"
        ),
        "set_icon",
        "get_icon"
    );
}

Item::Item() {

}

Item::~Item() {

}

uint32_t Item::get_id() const {
    return id;
}

void Item::set_id(uint32_t new_id) {
    id = new_id;
}

uint32_t Item::get_count() const {
    return count;
}

void Item::set_count(uint32_t new_count) {
    count = new_count;
}

uint32_t Item::get_stack_size() const {
    return stack_size;
}

void Item::set_stack_size(uint32_t new_stack_size) {
    stack_size = new_stack_size;
}

bool Item::get_can_drop() const {
    return can_drop;
}

void Item::set_can_drop(bool new_val) {
    can_drop = new_val;
}

String Item::get_display_name() const {
    return display_name;
}

void Item::set_display_name(String new_display_name) {
    display_name = new_display_name;
}

Ref<Texture2D> Item::get_icon() const {
    return icon;
}

void Item::set_icon(Ref<Texture2D> new_icon) {
    icon = new_icon;
}
