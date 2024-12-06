#include "../include/block.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void Block::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_texture"), &Block::get_texture);
	ClassDB::bind_method(D_METHOD("set_texture", "new_texture"), &Block::set_texture);

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
