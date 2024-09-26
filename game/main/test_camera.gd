class_name TestCamera extends Camera3D

var dir: Vector3

func _physics_process(delta: float) -> void:
	input()
	position += dir * delta * 4

func input() -> void:
	dir = Vector3()
	if Input.is_action_pressed("ui_left"):
		dir.x -= 1
	if Input.is_action_pressed("ui_right"):
		dir.x += 1
	if Input.is_action_pressed("ui_up"):
		dir.z -= 1
	if Input.is_action_pressed("ui_down"):
		dir.z += 1
