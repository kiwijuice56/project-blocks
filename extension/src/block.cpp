#include "../include/block.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void Block::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_texture"), &Block::get_texture);
	ClassDB::bind_method(D_METHOD("set_texture", "new_texture"), &Block::set_texture);
    ClassDB::bind_method(D_METHOD("get_transparent"), &Block::get_transparent);
	ClassDB::bind_method(D_METHOD("set_transparent", "new_val"), &Block::set_transparent);
    ClassDB::bind_method(D_METHOD("get_can_drop"), &Block::get_can_drop);
	ClassDB::bind_method(D_METHOD("set_can_drop", "new_val"), &Block::set_can_drop);
    ClassDB::bind_method(D_METHOD("get_break_sound"), &Block::get_break_sound);
	ClassDB::bind_method(D_METHOD("set_break_sound", "new_sound"), &Block::set_break_sound);
    ClassDB::bind_method(D_METHOD("get_step_sound"), &Block::get_step_sound);
	ClassDB::bind_method(D_METHOD("set_step_sound", "new_sound"), &Block::set_step_sound);
    ClassDB::bind_method(D_METHOD("get_drop_item"), &Block::get_drop_item);
	ClassDB::bind_method(D_METHOD("set_drop_item", "new_item"), &Block::set_drop_item);


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
        PropertyInfo(
            Variant::OBJECT,
            "break_sound",
            PROPERTY_HINT_RESOURCE_TYPE,
            "AudioStream"
        ),
        "set_break_sound",
        "get_break_sound"
    );

    ADD_PROPERTY(
        PropertyInfo(
            Variant::OBJECT,
            "step_sound",
            PROPERTY_HINT_RESOURCE_TYPE,
            "AudioStream"
        ),
        "set_step_sound",
        "get_step_sound"
    );

    ADD_PROPERTY(
        PropertyInfo(
            Variant::OBJECT,
            "drop_item",
            PROPERTY_HINT_RESOURCE_TYPE,
            "Item"
        ),
        "set_drop_item",
        "get_drop_item"
    );

    ADD_PROPERTY(
        PropertyInfo(Variant::BOOL, "transparent"),
        "set_transparent",
        "get_transparent"
    );

    ADD_PROPERTY(
        PropertyInfo(Variant::BOOL, "can_drop"),
        "set_can_drop",
        "get_can_drop"
    );


}

Block::Block() {
    held_item_scene = ResourceLoader::get_singleton()->load("res://main/items/held_item/held_block/held_block.tscn");
}

Block::~Block() {

}

Ref<Texture2D> Block::get_texture() const {
    return texture;
}

void Block::set_texture(Ref<Texture2D> new_texture) {
    texture = new_texture;
}

Ref<AudioStream> Block::get_break_sound() const {
    return break_sound;
}

void Block::set_break_sound(Ref<AudioStream> new_sound) {
    break_sound = new_sound;
}

Ref<AudioStream> Block::get_step_sound() const {
    return step_sound;
}

void Block::set_step_sound(Ref<AudioStream> new_sound) {
    step_sound = new_sound;
}

Ref<Item> Block::get_drop_item() const {
    return drop_item;
}

void Block::set_drop_item(Ref<Item> new_item) {
    drop_item = new_item;
}

bool Block::get_transparent() const {
    return transparent;
}

void Block::set_transparent(bool new_val) {
    transparent = new_val;
}

bool Block::get_can_drop() const {
    return can_drop;
}

void Block::set_can_drop(bool new_val) {
    can_drop = new_val;
}

