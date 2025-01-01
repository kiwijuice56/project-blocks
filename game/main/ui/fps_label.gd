class_name FPSLabel extends Label

func _ready() -> void:
	%Timer.timeout.connect(_on_timeout)

func _on_timeout() -> void:
	text = str(" ", Engine.get_frames_per_second())

func _input(event: InputEvent) -> void:
	if event.is_action_pressed("toggle_fps"):
		visible = not visible
