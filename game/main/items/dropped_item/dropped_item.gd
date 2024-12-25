class_name DroppedItem extends CharacterBody3D

@export var gravity: float = 16
@export var initial_speed: float = 32

var item: Item

func initialize(set_item: Item) -> void:
	# Center
	position += Vector3(0.5, 0.5, 0.5)
	velocity += initial_speed * Vector3(randf_range(-.8, .8), randf_range(0.2, 0.3), randf_range(-.8, .8)).normalized()
	
	item = set_item
	if item is Block:
		%Cube.visible = true
		%Cube.set_surface_override_material(0, %Cube.get_surface_override_material(0).duplicate())
		%Cube.get_surface_override_material(0).albedo_texture = item.texture
	else:
		%Cube.visible = true

func _physics_process(delta: float) -> void:
	velocity.x = lerp(velocity.x, 0.0, 8.0 * delta)
	velocity.z = lerp(velocity.z, 0.0, 8.0 * delta)
	if not is_on_floor():
		velocity.y -= delta * gravity
	else:
		velocity.y = 0
	move_and_slide()
