class_name TestWorld extends World

func _ready() -> void:
	for i in range(-2, 3):
		for j in range(-2, 3):
			center = Vector3(i, 0, j) * 16

func _physics_process(_delta: float) -> void:
	center = %TestCamera.position
