class_name PlaceEffect extends GPUParticles3D

func _on_finished() -> void:
	queue_free()

func initialize(block: Block) -> void:
	position += Vector3(0.5, 0.5, 0.5)
	material_override = material_override.duplicate()
	material_override.set("albedo_texture", block.texture)
	%AudioStreamPlayer3D.stream = block.step_sound
	%AudioStreamPlayer3D.play()
	
	%AudioStreamPlayer3D.finished.connect(_on_finished)
	finished.connect(_on_finished)
