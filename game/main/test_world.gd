class_name TestWorld extends World

func _ready() -> void:
	pass

var x: int = 0

func _physics_process(_delta: float) -> void:
	center = %TestCamera.position
	generate_from_queue(x % 2)
	x += 1
