class_name TestWorld extends World

func _ready() -> void:
	pass
	#for i in range(-3, 4):
	#	for j in range(3, 4):
	#		center = Vector3(i, 0, j) * 16
	#center = Vector3()
	#generate_from_queue(100)

var x: int = 0

func _physics_process(_delta: float) -> void:
	center = %Camera3D.position - Vector3(16, 0, 16)
	generate_from_queue(x % 2)
	x += 1
