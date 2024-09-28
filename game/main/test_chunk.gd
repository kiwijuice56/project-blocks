class_name TestChunk extends Chunk

var x: int = 0

func _ready() -> void:
	generate_data()

func _physics_process(_delta: float) -> void:
	generate_data()
	generate_mesh()
