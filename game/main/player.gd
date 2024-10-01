class_name Player extends CharacterBody3D

var gravity: float = 0#ProjectSettings.get_setting("physics/3d/default_gravity")
var speed: float = 32
var jump_speed: float = 6.5
var mouse_sensitivity: float = 0.01

func _ready() -> void:
	Input.mouse_mode = Input.MOUSE_MODE_CAPTURED

func _physics_process(delta: float):
	%Pointer.visible = %RayCast3D.is_colliding()
	if %RayCast3D.is_colliding():
		var collider: Node = %RayCast3D.get_collider()
		if is_instance_valid(collider):
			var chunk: Chunk = collider.get_parent()
			var block_position: Vector3 = %RayCast3D.get_collision_point() - %RayCast3D.get_collision_normal() * 0.25
			
			if Input.is_action_just_pressed("main_interact"):
				chunk.remove_block_at(block_position)
			block_position -= chunk.global_position
			block_position.x = int(block_position.x)
			block_position.y = int(block_position.y)
			block_position.z = int(block_position.z)
			
			%Pointer.global_position = chunk.global_position + block_position + Vector3(0.5, 0.5, 0.5)
	
	velocity.y += -gravity * delta
	
	var input: Vector2 = Input.get_vector("left", "right", "up", "down")
	var movement_dir: Vector3 = transform.basis * Vector3(input.x, 0, input.y)
	velocity.x = movement_dir.x * speed
	velocity.z = movement_dir.z * speed

	move_and_slide()
	if is_on_floor() and Input.is_action_pressed("jump"):
		velocity.y = jump_speed

func _input(event: InputEvent):
	if event is InputEventMouseMotion and Input.mouse_mode == Input.MOUSE_MODE_CAPTURED:
		rotate_y(-event.relative.x * mouse_sensitivity)
		%Camera3D.rotate_x(-event.relative.y * mouse_sensitivity)
		%Camera3D.rotation.x = clampf($Camera3D.rotation.x, -deg_to_rad(70), deg_to_rad(70))
	if event.is_action_pressed("ui_cancel", false):
		if Input.mouse_mode == Input.MOUSE_MODE_CAPTURED:
			Input.mouse_mode = Input.MOUSE_MODE_VISIBLE
		else:
			Input.mouse_mode = Input.MOUSE_MODE_CAPTURED
