class_name DroppedItem extends CharacterBody3D

@export var gravity: float = 16
@export var initial_speed: float = 32
@export var gravity_range: float = 4.0
@export var horizontal_dampening: float = 8.0
@export var merge_time: float = 0.5

var merge_lock: bool = false

var item: Item

func _ready() -> void:
	%DetectionArea3D.area_entered.connect(_on_area_entered)

func _on_area_entered(new_area: Area3D) -> void:
	if merge_lock:
		return
	
	if not new_area.get_parent() is DroppedItem:
		return
	
	var other: DroppedItem = new_area.get_parent() as DroppedItem
	
	if not is_instance_valid(other) or other.merge_lock:
		return
	
	if other.item.count > item.count or item.id != other.item.id:
		return
	
	other.merge_lock = true
	merge_lock = true
	
	absorb.call_deferred(other)

func _physics_process(delta: float) -> void:
	velocity.x = lerp(velocity.x, 0.0, horizontal_dampening * delta)
	velocity.z = lerp(velocity.z, 0.0, horizontal_dampening * delta)
	if not is_on_floor():
		velocity.y -= delta * gravity
	else:
		velocity.y = 0
	move_and_slide()

# Called when instantiated 
func initialize(set_item: Item) -> void:
	# Movement 
	gravity += randf_range(-1, 1) * gravity_range
	position += Vector3(0.5, 0.5, 0.5)
	velocity = initial_speed * Vector3(randf_range(-1., 1.), 1., randf_range(-1., 1.)).normalized()
	
	item = set_item
	
	# Visuals
	if item is Block:
		%Cube.visible = true
		%Cube.set_surface_override_material(0, %Cube.get_surface_override_material(0).duplicate())
		%Cube.get_surface_override_material(0).albedo_texture = item.texture
	else:
		%Cube.visible = true

func absorb(other: DroppedItem) -> void:
	toggle_physics(false)
	other.toggle_physics(false)
	
	var center: Vector3 = (global_position + other.global_position) / 2
	var tween: Tween = get_tree().create_tween().set_parallel(true)
	tween.tween_property(self, "global_position", center, merge_time)
	tween.tween_property(other, "global_position", center, merge_time)
	await tween.finished
	
	toggle_physics(true)
	other.toggle_physics(true)
	
	var total_count = other.item.count + item.count
	item.count = min(total_count, item.stack_size)
	other.item.count = total_count - item.count
	
	if other.item.count == 0:
		other.queue_free()
	else:
		other.merge_lock = false
	merge_lock = false

func toggle_physics(enable: bool) -> void:
	%CollisionShape3D.disabled = not enable
	set_physics_process(enable)
