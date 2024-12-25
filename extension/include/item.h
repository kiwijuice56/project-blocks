
#pragma once

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/classes/ref_counted.hpp>

namespace godot {

class Item : public Resource {
	GDCLASS(Item, Resource);

private:

protected:
    uint32_t id = 0;
    String display_name;
    Ref<Texture2D> icon;
    uint32_t stack_size = 64;
    uint32_t count = 1;

	static void _bind_methods();

    void simulate(uint64_t state);

public:
	Item();
	~Item();

    uint32_t get_id() const;
    void set_id(uint32_t new_id);

    uint32_t get_count() const;
    void set_count(uint32_t new_count);

    uint32_t get_stack_size() const;
    void set_stack_size(uint32_t new_stack_size);

    String get_display_name() const;
    void set_display_name(String new_display_name);

    Ref<Texture2D> get_icon() const;
    void set_icon(Ref<Texture2D> new_texture);
};

}