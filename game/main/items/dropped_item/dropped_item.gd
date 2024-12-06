class_name DroppedItem extends Area3D

var item: Item

func initialize(set_item: Item) -> void:
	item = set_item
	if item is Block:
		%Cube.visible = true
		%Cube.get_surface_override_material(0).albedo_texture = item.texture
	else:
		%Cube.visible = true
	
