class_name TestChunk extends Chunk

func _ready() -> void:
	set_physics_process(false)
	await main_noise_texture.changed
	set_physics_process(true)

func _physics_process(_delta: float) -> void:
	generate_data()
	generate_mesh()
