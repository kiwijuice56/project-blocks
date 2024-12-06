#include "../include/item.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void Item::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_id"), &Item::get_id);
	ClassDB::bind_method(D_METHOD("set_id", "new_id"), &Item::set_id);
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
