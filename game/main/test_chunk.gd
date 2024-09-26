@tool
class_name TestChunk extends Chunk

func _ready() -> void:
	generate_chunk()
	generate_mesh()
	
	mesh.surface_set_material(0, preload("res://main/block_material.tres"))
