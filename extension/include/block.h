
#pragma once

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/classes/ref_counted.hpp>

namespace godot {

class Block : public Resource {
	GDCLASS(Block, Resource)

private:
    uint8_t id = 0;
    String display_name;
    Ref<Texture2D> texture;

protected:
	static void _bind_methods();

    void simulate(uint64_t state);

public:
	Block();
	~Block();

    uint8_t get_id() const;
    void set_id(uint8_t new_id);

    String get_display_name() const;
    void set_display_name(String new_display_name);

    Ref<Texture2D> get_texture() const;
    void set_texture(Ref<Texture2D> new_texture);
};

}