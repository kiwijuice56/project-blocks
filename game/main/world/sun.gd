class_name Sun extends DirectionalLight3D

@export_range(1, 120, 1, "suffix:min") var day_length: float

var day_scale: float = 0.3
var is_day: bool = true

func _process(delta: float) -> void:
	if is_day:
		var time_scale: float = 1.0 / (60 * day_length)
		day_scale += delta * time_scale
		
		rotation_degrees.x = -int(day_scale * 180.0 * 4.0) / 4.0
		if day_scale >= 1:
			print(day_scale)
			day_scale = 0.0
