class_name Step extends Behavior

@export var step_player_scene: PackedScene = preload("res://main/entity/behaviors/step/step_player.tscn")

func _ready() -> void:
	super._ready()
	entity.hit_ground.connect(_on_hit_ground)

func _on_hit_ground() -> void:
	step()

func step() -> void:
	if not entity.is_on_floor():
		return
	var collision: KinematicCollision3D = entity.get_last_slide_collision()
	if collision == null:
		return
	
	var index: int = 0
	for i in collision.get_collision_count():
		if collision.get_normal(i).is_equal_approx(Vector3.UP):
			index = i
	var floor_point: Vector3 = collision.get_position(index) - Vector3(0, 0.01, 0)
	if not Ref.world.is_position_loaded(floor_point):
		return
	
	if Ref.world.get_block_type_at(floor_point).id == 0: 
		for nudge in [Vector3(1, 0, 0), Vector3(-1, 0, 0), Vector3(0, 0, 1), Vector3(0, 0, -1)]:
			var new_point: Vector3 = floor_point + nudge * 0.001
			if not Ref.world.is_position_loaded(new_point):
				continue
			if Ref.world.get_block_type_at(new_point).id == 0:
				continue
			floor_point = new_point
			break
	
	var new_player: AudioStreamPlayer3D = step_player_scene.instantiate()
	get_tree().get_root().add_child(new_player)
	new_player.global_position = floor_point
	new_player.stream = Ref.world.get_block_type_at(floor_point).step_sound
	new_player.playing = true
	new_player.finished.connect(new_player.queue_free)
