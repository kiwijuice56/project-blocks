class_name Creature extends CharacterBody3D

@export var dropped_item_scene: PackedScene

@export var max_health: int = 10
@export var pick_up_items: bool = true
@export var repickup_delay: float = 1.0

var movement_enabled: bool = true

var held_item: ItemState
var health: float

var in_air: bool = false

func _ready() -> void:
	%ItemArea3D.area_entered.connect(_on_dropped_item_entered)

func _physics_process(_delta: float) -> void:
	if not is_on_floor():
		in_air = true
	if in_air and is_on_floor():
		in_air = false
		step()

func _on_dropped_item_entered(area: Area3D) -> void:
	if not area.get_parent() is DroppedItem:
		return
	var dropped_item: DroppedItem = area.get_parent() as DroppedItem
	if not dropped_item.can_collect:
		return
	
	var remaining_item: Item = %Inventory.accept(dropped_item.item)
	if remaining_item == null:
		dropped_item.collect()

func drop_item(item: ItemState, drop_position: Vector3, initial_velocity: Vector3) -> void:
	var new_item: DroppedItem = dropped_item_scene.instantiate()
	new_item.collect_delay_time = repickup_delay
	get_parent().add_child(new_item)
	new_item.initialize(item)
	new_item.global_position = drop_position
	new_item.velocity = initial_velocity

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
	
	var new_player: AudioStreamPlayer3D = %StepPlayer3D.duplicate()
	add_child(new_player)
	new_player.global_position = floor_point
	new_player.stream = Ref.world.get_block_type_at(floor_point).step_sound
	new_player.playing = true
	new_player.finished.connect(new_player.queue_free)
