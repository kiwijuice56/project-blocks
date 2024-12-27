
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

	bool transparent = false;

	static void _bind_methods();

public:
	Block();
	~Block();

    Ref<Texture2D> get_texture() const;
    void set_texture(Ref<Texture2D> new_texture);

	Ref<AudioStream> get_break_sound() const;
    void set_break_sound(Ref<AudioStream> new_sound);

	Ref<AudioStream> get_step_sound() const;
    void set_step_sound(Ref<AudioStream> new_sound);

	bool get_transparent() const;
    void set_transparent(bool new_val);
};

}