class_name WaterCover extends ColorRect

func _ready() -> void:
	Ref.player_camera.under_water_status_changed.connect(_on_under_water_status_changed)

func _on_under_water_status_changed(new_val: bool) -> void:
	visible = new_val
