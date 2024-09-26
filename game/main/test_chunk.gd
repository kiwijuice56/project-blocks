@tool
class_name TestChunk extends Chunk

func _ready() -> void:
	print("!")
	generate_chunk()
	generate_mesh()
	
	var new_material: Material = StandardMaterial3D.new()
	new_material.albedo_texture = preload("res://main/texture_atlas.png")
	new_material.texture_filter = BaseMaterial3D.TEXTURE_FILTER_NEAREST
	mesh.surface_set_material(0, new_material)
