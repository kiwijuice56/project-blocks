
#pragma once

#include "item.h"

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/classes/ref_counted.hpp>

namespace godot {

class Block : public Item {
	GDCLASS(Block, Item);

private:

protected:
    Ref<Texture2D> texture;

	static void _bind_methods();

public:
	Block();
	~Block();

    Ref<Texture2D> get_texture() const;
    void set_texture(Ref<Texture2D> new_texture);
};

}