class_name Player extends Creature

@export var world: World
@export var selected_block: Block

@export_group("Toggles")
@export var flying: bool = false
@export var sprint_toggle: bool = false

@export_group("Acceleration")
@export var ground_accel: float = 56.0
@export var air_accel: float = 48.0
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
@export var sprint_fov_scale: float = 1.15
@export var sprint_fov_animation_speed: float = 9.0
@export var sprint_difference_threshold: float = 0.1
@export var minimum_sprint_speed: float = 1.0
@export var floor_place_deadzone: float = 0.6
@export var floor_place_down_angle: float = -0.6

var is_sprinting_requested: bool = false
var is_sprinting: bool = false

func _ready() -> void:
	Input.mouse_mode = Input.MOUSE_MODE_CAPTURED

func _process(delta: float) -> void:
	%Camera3D.fov = lerp(%Camera3D.fov, fov * (sprint_fov_scale if is_sprinting else 1.0), sprint_fov_animation_speed * delta)
	
	# Block picking logic
	if %InteractRayCast3D.is_colliding():
		var selected_position: Vector3 = (%InteractRayCast3D.get_collision_point() - %InteractRayCast3D.get_collision_normal() * 0.5 - Vector3(1, 1, 1)).ceil()
		var block_position: Vector3i = Vector3i(selected_position)
		var place_position: Vector3i = block_position + Vector3i(%InteractRayCast3D.get_collision_normal())
		
		%PlacementCheckShapeCast3D.global_position = Vector3(place_position) + Vector3(0.5, 0.5, 0.5)
		%PlacementCheckShapeCast3D.force_shapecast_update()
		
		if Input.is_action_pressed("main_interact") and world.is_position_loaded(block_position):
			world.break_block_at(block_position, true)
		if Input.is_action_just_pressed("secondary_interact") and world.is_position_loaded(place_position) and not %PlacementCheckShapeCast3D.is_colliding():
			world.place_block_at(place_position, selected_block.id)
	elif %FloorRayCast3D.is_colliding() and Input.is_action_just_pressed("secondary_interact"):
			var floor_position: Vector3i = Vector3i((global_position - Vector3(0, 0.25, 0)).floor())
			var look_direction: Vector3 = -%Camera3D.get_global_transform().basis.z
			
			if look_direction.y > floor_place_down_angle:
				return
			
			var flat_look_direction: Vector3 = -get_global_transform().basis.z
			if abs(flat_look_direction.x) < floor_place_deadzone and abs(flat_look_direction.z) < floor_place_deadzone:
				return
			
			if abs(flat_look_direction.x) < abs(flat_look_direction.z):
				flat_look_direction = Vector3(0, 0, sign(flat_look_direction.z))
			else:
				flat_look_direction = Vector3(sign(flat_look_direction.x), 0, 0)
			
			var floor_block_position: Vector3i = floor_position + Vector3i(flat_look_direction)
			
			if world.is_position_loaded(floor_block_position):
				world.place_block_at(floor_block_position, selected_block.id)
	
	# Update pointer visual
	%InteractRayCast3D.force_raycast_update()
	%BlockOutline.visible = %InteractRayCast3D.is_colliding()
	if %InteractRayCast3D.is_colliding():
		var selected_position: Vector3 = (%InteractRayCast3D.get_collision_point() - %InteractRayCast3D.get_collision_normal() * 0.5 - Vector3(1, 1, 1)).ceil()
		%BlockOutline.global_position = selected_position + Vector3(0.5, 0.5, 0.5)

func _physics_process(delta: float):
	# Movement logic
	var input: Vector2 = Input.get_vector("left", "right", "up", "down")
	var movement_dir: Vector3 = transform.basis * Vector3(input.x, 0, input.y)
	var look_direction: Vector3 = -get_global_transform().basis.z
	
	if not sprint_toggle:
		is_sprinting_requested = Input.is_action_pressed("sprint")
	
	is_sprinting = is_sprinting_requested 
	
	if velocity.length() < minimum_sprint_speed or look_direction.normalized().dot(movement_dir.normalized()) < sprint_difference_threshold:
		is_sprinting = false
	
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

func _input(event: InputEvent) -> void:
	if event is InputEventMouseMotion and Input.mouse_mode == Input.MOUSE_MODE_CAPTURED:
		rotate_y(-event.relative.x * mouse_sensitivity)
		%Camera3D.rotate_x(-event.relative.y * mouse_sensitivity)
		%Camera3D.rotation.x = clampf($Camera3D.rotation.x, -deg_to_rad(90), deg_to_rad(90))
	
	if sprint_toggle and event.is_action_pressed("sprint", false):
		is_sprinting_requested = not is_sprinting_requested
	
	if event.is_action_pressed("ui_cancel", false):
		if Input.mouse_mode == Input.MOUSE_MODE_CAPTURED:
			Input.mouse_mode = Input.MOUSE_MODE_VISIBLE
		else:
			Input.mouse_mode = Input.MOUSE_MODE_CAPTURED
