class_name TestWorld extends World

func _ready() -> void:
	pass
	set_physics_process(false)
	await main_noise_texture.changed
	set_physics_process(true)
	
	for i in range(-3, 4):
		for j in range(3, 4):
			center = Vector3(i, 0, j) * 8
	center = Vector3()
	generate_from_queue(512)

var x: int = 0

func _physics_process(_delta: float) -> void:
	center = %Camera3D.position - Vector3(4, 0, 4)
	generate_from_queue(1)
