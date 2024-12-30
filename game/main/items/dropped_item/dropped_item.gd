class_name DroppedItem extends CharacterBody3D

@export var gravity: float = 16
@export var initial_speed: float = 32
@export var gravity_range: float = 4.0
@export var horizontal_dampening: float = 8.0
@export var merge_time: float = 0.5
@export var swim_speed: float = 8.0
@export var awaken_distnace: float = 4.0

var item: Item

enum { MERGE, SWIM, IDLE, SLEEPING, COLLECTED }

var state: int = IDLE

func _ready() -> void:
	%DetectionArea3D.area_entered.connect(_on_area_entered)

func _on_area_entered(new_area: Area3D) -> void:
	if state != IDLE:
		return
	
	if not new_area.get_parent() is DroppedItem:
		return
	
	var other: DroppedItem = new_area.get_parent() as DroppedItem
	
	if not is_instance_valid(other) or other.state != IDLE:
		return
	
	if other.item.count > item.count or item.id != other.item.id:
		return
	
	state = MERGE
	other.state = MERGE
	
	absorb.call_deferred(other)

func _on_block_placed(block_position: Vector3) -> void:
	if (state == IDLE or state == SLEEPING) and block_position.distance_to(global_position) < awaken_distnace:
		check_swim()

func _on_block_broken(block_position: Vector3) -> void:
	if state == SLEEPING and block_position.distance_to(global_position) < awaken_distnace:
		state = IDLE
		toggle_physics(true)
		check_swim()

func _physics_process(delta: float) -> void:
	if not Ref.world.is_position_loaded(global_position):
		return
	
	move_and_slide()
	
	if state == SWIM:
		if Ref.world.get_block_type_at(global_position).id == 0:
			state = IDLE
			toggle_collision(true)
			velocity = Vector3()
	
	if state == IDLE:
		# Basic gravity and movement dampening
		velocity.x = lerp(velocity.x, 0.0, horizontal_dampening * delta)
		velocity.z = lerp(velocity.z, 0.0, horizontal_dampening * delta)
		if not is_on_floor():
			velocity.y -= delta * gravity
		else:
			velocity.y = 0
			state = SLEEPING
			toggle_physics(false)

# Called when instantiated 
func initialize(set_item: Item) -> void:
	Ref.world.block_placed.connect(_on_block_placed)
	Ref.world.block_broken.connect(_on_block_broken)
	
	# Movement 
	gravity += randf_range(-1, 1) * gravity_range
	position += Vector3(0.5, 0.5, 0.5)
	velocity = initial_speed * Vector3(randf_range(-1., 1.), 1., randf_range(-1., 1.)).normalized()
	
	item = set_item.duplicate()
	
	# Visuals
	if item is Block:
		%Cube.visible = true
		%Cube.set_surface_override_material(0, %Cube.get_surface_override_material(0).duplicate())
		%Cube.get_surface_override_material(0).albedo_texture = item.texture
	else:
		%Cube.visible = true

func absorb(other: DroppedItem) -> void:
	# Just in case...
	if not is_instance_valid(other) or other.state != MERGE:
		state = IDLE
		return
	
	toggle_physics(false)
	other.toggle_physics(false)
	
	var center: Vector3 = (global_position + other.global_position) / 2
	var tween: Tween = get_tree().create_tween().set_parallel(true)
	tween.tween_property(self, "global_position", center, merge_time)
	tween.tween_property(other, "global_position", center, merge_time)
	await tween.finished
	
	# If collected mid-merge, just return
	if state != MERGE:
		return
	
	# Was other collected mid-merge?
	if not is_instance_valid(other) or other.state != MERGE:
		toggle_physics(true)
		state = IDLE
		check_swim()
		return
	
	toggle_physics(true)
	other.toggle_physics(true)
	
	# Distribute items
	var total_count = other.item.count + item.count
	item.count = min(total_count, item.stack_size)
	other.item.count = total_count - item.count
	if other.item.count == 0:
		other.queue_free()
	else:
		other.state = IDLE
		other.check_swim()
	
	state = IDLE
	check_swim()

func collect() -> void:
	toggle_physics.call_deferred(false)
	state = COLLECTED
	%CollectAnimationPlayer.play("collect")
	await %CollectAnimationPlayer.animation_finished
	queue_free()

func toggle_collision(enable: bool) -> void:
	%CollisionShape3D.disabled = not enable

func toggle_physics(enable: bool) -> void:
	toggle_collision(enable)
	set_physics_process(enable)

# Checks if this item is inside of a block, and initiate a swim outwards if so
func check_swim() -> void:
	if Ref.world.get_block_type_at(global_position).id != 0:
		state = SWIM
		var swim_dir: Vector3 = Vector3.UP
		var spots: Array[Vector3] = [Vector3.UP, Vector3.LEFT, Vector3.RIGHT, Vector3.FORWARD, Vector3.BACK, Vector3.DOWN]
		for spot in spots:
			if not Ref.world.is_position_loaded(global_position + spot):
				continue
			if Ref.world.get_block_type_at(global_position + spot).id == 0:
				swim_dir = spot
				break
		velocity = swim_speed * swim_dir
		toggle_physics(true)
		toggle_collision(false)
