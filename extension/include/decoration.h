
#pragma once

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/classes/ref_counted.hpp>

namespace godot {

class Decoration : public Resource {
	GDCLASS(Decoration, Resource);

private:

protected:
    Vector3i size;
    PackedInt32Array blocks;

	static void _bind_methods();

public:
    Vector3i position;

	Decoration();
	~Decoration();

    PackedInt32Array get_blocks() const;
    void set_blocks(PackedInt32Array new_blocks);

	Vector3i get_size() const;
    void set_size(Vector3i new_size);
};

}