#include "../include/block.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void Block::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_id"), &Block::get_id);
	ClassDB::bind_method(D_METHOD("set_id", "new_id"), &Block::set_id);
    ClassDB::bind_method(D_METHOD("get_display_name"), &Block::get_display_name);
	ClassDB::bind_method(D_METHOD("set_display_name", "new_display_name"), &Block::set_display_name);
    ClassDB::bind_method(D_METHOD("get_texture"), &Block::get_texture);
	ClassDB::bind_method(D_METHOD("set_texture", "new_texture"), &Block::set_texture);

    ClassDB::add_property(
        "Block",
        PropertyInfo(Variant::INT, "id"),
        "set_id",
        "get_id"
    );

    ClassDB::add_property(
        "Block",
        PropertyInfo(Variant::STRING, "display_name"),
        "set_display_name",
        "get_display_name"
    );

    ClassDB::add_property(
        "Block",
        PropertyInfo(
            Variant::OBJECT,
            "texture",
            PROPERTY_HINT_RESOURCE_TYPE,
            "Texture2D"
        ),
        "set_texture",
        "get_texture"
    );
}

Block::Block() {

}

Block::~Block() {

}

uint8_t Block::get_id() const {
    return id;
}

void Block::set_id(uint8_t new_id) {
    id = new_id;
}

String Block::get_display_name() const {
    return display_name;
}

void Block::set_display_name(String new_display_name) {
    display_name = new_display_name;
}

Ref<Texture2D> Block::get_texture() const {
    return texture;
}

void Block::set_texture(Ref<Texture2D> new_texture) {
    texture = new_texture;
}

void Block::simulate(uint64_t state) {

}