class_name MainWorld extends World

func _ready() -> void:
	set_physics_process(false)
	initialize()
	await main_noise_texture.changed
	set_physics_process(true)

func _physics_process(_delta: float) -> void:
	set_loaded_region_center(%Player.global_position)
