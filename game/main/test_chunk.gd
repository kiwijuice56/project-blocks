class_name TextChunk extends Chunk

func _ready() -> void:
	for i in range(16):
		for j in range(128):
			for k in range(16):
				generate_cube(Vector3(i, j, k))
	generate_mesh()
