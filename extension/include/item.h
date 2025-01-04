
#pragma once

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/packed_scene.hpp>


namespace godot {

class Item : public Resource {
	GDCLASS(Item, Resource);

protected:
	static void _bind_methods();

public:
    uint32_t id = 0;
    String internal_name;
    String display_name;
    Ref<Texture2D> icon;
    Ref<PackedScene> held_item_scene;
    uint32_t stack_size = 64;

	Item();
	~Item();

    uint32_t get_id() const;
    void set_id(uint32_t new_id);

    uint32_t get_stack_size() const;
    void set_stack_size(uint32_t new_stack_size);

    String get_display_name() const;
    void set_display_name(String new_display_name);

    String get_internal_name() const;
    void set_internal_name(String new_internal_name);

    Ref<Texture2D> get_icon() const;
    void set_icon(Ref<Texture2D> new_texture);

    Ref<PackedScene> get_held_item_scene() const;
    void set_held_item_scene(Ref<PackedScene> new_scene);
};

}