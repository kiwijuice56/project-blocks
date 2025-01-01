
#pragma once

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/classes/ref_counted.hpp>

namespace godot {

class Decoration : public Resource {
	GDCLASS(Decoration, Resource);

protected:
	static void _bind_methods();

public:
    String internal_name;

    Vector3i size;
    PackedInt32Array blocks;

    Vector3i position;

	Decoration();
	~Decoration();

    PackedInt32Array get_blocks() const;
    void set_blocks(PackedInt32Array new_blocks);

    String get_internal_name() const;
    void set_internal_name(String new_internal_name);

	Vector3i get_size() const;
    void set_size(Vector3i new_size);
};

}