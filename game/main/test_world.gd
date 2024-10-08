class_name TestWorld extends World

var x = 0

func _ready() -> void:
	set_physics_process(false)
	initialize()
	await main_noise_texture.changed
	set_physics_process(true)

func _physics_process(_delta: float) -> void:
	center = %Player.global_position - Vector3(16, 16, 16)
