#include "../include/block.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void Block::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_texture"), &Block::get_texture);
	ClassDB::bind_method(D_METHOD("set_texture", "new_texture"), &Block::set_texture);
    ClassDB::bind_method(D_METHOD("get_transparent"), &Block::get_transparent);
	ClassDB::bind_method(D_METHOD("set_transparent", "new_val"), &Block::set_transparent);

    ADD_PROPERTY(
        PropertyInfo(
            Variant::OBJECT,
            "texture",
            PROPERTY_HINT_RESOURCE_TYPE,
            "Texture2D"
        ),
        "set_texture",
        "get_texture"
    );

    ADD_PROPERTY(
        PropertyInfo(Variant::BOOL, "transparent"),
        "set_transparent",
        "get_transparent"
    );
}

Block::Block() {

}

Block::~Block() {

}

Ref<Texture2D> Block::get_texture() const {
    return texture;
}

void Block::set_texture(Ref<Texture2D> new_texture) {
    texture = new_texture;
}

bool Block::get_transparent() const {
    return transparent;
}

void Block::set_transparent(bool new_val) {
    transparent = new_val;
}

