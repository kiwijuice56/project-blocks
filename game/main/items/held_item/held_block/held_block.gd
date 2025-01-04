class_name HeldBlock extends HeldItem

func initialize(set_item: Item, set_holder: Entity) -> void:
	super.initialize(set_item, set_holder)
	%Cube.set_surface_override_material(0, %Cube.get_surface_override_material(0).duplicate())
	%Cube.get_surface_override_material(0).albedo_texture = ItemMap.map(item.id).texture
	
