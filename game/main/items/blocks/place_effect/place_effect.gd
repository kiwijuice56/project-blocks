class_name PlaceEffect extends GPUParticles3D

var finish_amount: int = 0

func _on_finished() -> void:
	finish_amount += 1
	if finish_amount == 2:
		queue_free()

func initialize(block: Block) -> void:
	position += Vector3(0.5, 0.5, 0.5)
	# emitting = true
	material_override = material_override.duplicate()
	material_override.set("albedo_texture", block.texture)
	%AudioStreamPlayer3D.stream = block.step_sound
	%AudioStreamPlayer3D.play()
	
	%AudioStreamPlayer3D.finished.connect(_on_finished)
	finished.connect(_on_finished)
