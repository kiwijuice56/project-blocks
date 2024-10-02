class_name FPSLabel extends Label

func _ready() -> void:
	%Timer.timeout.connect(_on_timeout)

func _on_timeout() -> void:
	text = str(" ", Engine.get_frames_per_second())
