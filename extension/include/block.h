
#pragma once

#include "item.h"

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/audio_stream.hpp>

namespace godot {

class Block : public Item {
	GDCLASS(Block, Item);

private:

protected:
    Ref<Texture2D> texture;
	Ref<AudioStream> break_sound;
	Ref<AudioStream> step_sound;
	Ref<Item> drop_item;

	bool transparent = false;
	bool can_drop = true;

	static void _bind_methods();

public:
	uint32_t index = 0;

	Block();
	~Block();

    Ref<Texture2D> get_texture() const;
    void set_texture(Ref<Texture2D> new_texture);

	Ref<AudioStream> get_break_sound() const;
    void set_break_sound(Ref<AudioStream> new_sound);

	Ref<AudioStream> get_step_sound() const;
    void set_step_sound(Ref<AudioStream> new_sound);

	Ref<Item> get_drop_item() const;
    void set_drop_item(Ref<Item> new_item);

	bool get_can_drop() const;
    void set_can_drop(bool new_val);

	bool get_transparent() const;
    void set_transparent(bool new_val);
};

}