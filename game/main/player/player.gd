class_name Player extends CharacterBody3D

@export var world: World

@export_group("Toggles")
@export var flying: bool = false
@export var sprint_toggle: bool = false

@export_group("Acceleration")
@export var ground_accel: float = 0.98
@export var air_accel: float = 0.8
@export var gravity: float = 32

@export_group("Speed")
@export var sprint_speed: float = 5.2
@export var jump_speed_multiplier: float = 1.1
@export var fly_speed: float = 16
@export var walk_speed: float = 3.9

@export_group("Impulse")
@export var jump_impulse: float = 9
@export var fly_impulse: float = 32

@export_group("Other Settings")
@export var mouse_sensitivity: float = 0.01
@export var fov: float = 86
@export var sprint_fov_scale: float = 1.1
@export var sprint_fov_animation_speed: float = 0.25

var is_sprinting: bool = false

func _ready() -> void:
	Input.mouse_mode = Input.MOUSE_MODE_CAPTURED

func _process(delta: float) -> void:
	%Camera3D.fov = lerp(%Camera3D.fov, fov * (sprint_fov_scale if is_sprinting else 1.0), sprint_fov_animation_speed * delta)

func _physics_process(delta: float):
	# Block picking logic
	%Pointer.visible = %RayCast3D.is_colliding()
	if %RayCast3D.is_colliding():
		var collider: Node = %RayCast3D.get_collider()
		if is_instance_valid(collider):
			var chunk: Chunk = collider.get_parent()
			var block_position: Vector3i = Vector3i((%RayCast3D.get_collision_point() - %RayCast3D.get_collision_normal() * 0.25).floor())
			%Pointer.global_position = Vector3(block_position) + Vector3(0.5, 0.5, 0.5)
			
			if Input.is_action_just_pressed("main_interact"):
				chunk.remove_block_at.call_deferred(block_position)
			if Input.is_action_just_pressed("secondary_interact"):
				var place_position: Vector3i = block_position + Vector3i(%RayCast3D.get_collision_normal().floor())
				if world.is_position_loaded(place_position):
					world.get_chunk_at(16 * (place_position / 16)).place_block_at(place_position % 16, 2)
					world.get_chunk_at(16 * (place_position / 16)).generate_mesh()
	
	# Movement logic
	var input: Vector2 = Input.get_vector("left", "right", "up", "down")
	var movement_dir: Vector3 = transform.basis * Vector3(input.x, 0, input.y)
	
	if not sprint_toggle:
		is_sprinting = Input.is_action_pressed("sprint")
	
	var target_speed: float = walk_speed
	var accel: float = ground_accel
	
	if is_sprinting:
		target_speed = sprint_speed
	if flying:
		target_speed = fly_speed
	
	if not is_on_floor():
		accel = air_accel
		target_speed *= jump_speed_multiplier
	
	var temp: float = velocity.y
	velocity = lerp(velocity, movement_dir * target_speed, accel * delta)
	
	# Exclude vertical direction from xz lerping
	velocity.y = temp - gravity * delta
	
	if (flying or is_on_floor()) and Input.is_action_pressed("jump"):
		velocity.y = fly_impulse if flying else jump_impulse 
	
	if not world.is_position_loaded(velocity * delta + global_position):
		velocity = Vector3()
	
	move_and_slide()
	

func _input(event: InputEvent):
	if event is InputEventMouseMotion and Input.mouse_mode == Input.MOUSE_MODE_CAPTURED:
		rotate_y(-event.relative.x * mouse_sensitivity)
		%Camera3D.rotate_x(-event.relative.y * mouse_sensitivity)
		%Camera3D.rotation.x = clampf($Camera3D.rotation.x, -deg_to_rad(90), deg_to_rad(90))
	
	if sprint_toggle and event.is_action_pressed("sprint", false):
		is_sprinting = not is_sprinting
	
	if event.is_action_pressed("ui_cancel", false):
		if Input.mouse_mode == Input.MOUSE_MODE_CAPTURED:
			Input.mouse_mode = Input.MOUSE_MODE_VISIBLE
		else:
			Input.mouse_mode = Input.MOUSE_MODE_CAPTURED
