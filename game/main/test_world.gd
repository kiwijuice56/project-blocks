class_name TestWorld extends World

func _ready() -> void:
	set_physics_process(false)
	await main_noise_texture.changed
	set_physics_process(true)

func _physics_process(_delta: float) -> void:
	center = %Camera3D.global_position - Vector3(4, 0, 4)
