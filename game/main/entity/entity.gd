class_name Entity extends CharacterBody3D

@export_group("Stats")
@export var max_health: int = 10
@export var strength: int

@export_group("Behavior")
@export var pick_up_items: bool = true
@export var repickup_delay: float = 1.0

@export_group("Scenes")
@export var step_player_scene: PackedScene

var held_item: ItemState

var health: int

var movement_enabled: bool = true

var in_air: bool = false

func _physics_process(_delta: float) -> void:
	if not is_on_floor():
		in_air = true
	if in_air and is_on_floor():
		in_air = false
		step()

func step() -> void:
	if not is_on_floor():
		return
	var collision: KinematicCollision3D = get_last_slide_collision()
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
	add_child(new_player)
	new_player.global_position = floor_point
	new_player.stream = Ref.world.get_block_type_at(floor_point).step_sound
	new_player.playing = true
	new_player.finished.connect(new_player.queue_free)

func attack() -> void:
	pass
