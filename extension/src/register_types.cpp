#include "../include/register_types.h"

#include "../include/chunk.h"
#include "../include/world.h"
#include "../include/block.h"
#include "../include/item.h"
#include "../include/item_state.h"
#include "../include/decoration.h"
#include "../include/generator.h"
#include "../include/debug_generator.h"
#include "../include/default_generator.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_gdblocks_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	GDREGISTER_CLASS(Item);
	GDREGISTER_CLASS(ItemState);
	GDREGISTER_CLASS(Block);
	GDREGISTER_CLASS(Decoration);
	GDREGISTER_CLASS(Chunk);
	GDREGISTER_CLASS(Generator);
	GDREGISTER_CLASS(DebugGenerator);
	GDREGISTER_CLASS(DefaultGenerator);
	GDREGISTER_CLASS(World);
}

void uninitialize_gdblocks_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
}

extern "C" {
GDExtensionBool GDE_EXPORT gdblocks_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
	godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

	init_obj.register_initializer(initialize_gdblocks_module);
	init_obj.register_terminator(uninitialize_gdblocks_module);
	init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

	return init_obj.init();
}
}