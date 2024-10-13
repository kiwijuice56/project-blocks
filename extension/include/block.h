
#pragma once

#include "item.h"

namespace godot {

class Block : public Item {
	GDCLASS(Block, Item)

private:
    Ref<Texture2D> texture;

protected:
	static void _bind_methods();

public:
	Block();
	~Block();

    Ref<Texture2D> get_texture() const;
    void set_texture(Ref<Texture2D> new_texture);
};

}