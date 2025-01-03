class_name Entity extends CharacterBody3D

@export_group("Stats")
@export var max_health: int = 10
@export var strength: int

@export_group("Movement")
@export var speed: float = 3.9
@export var ascend_impulse: float = 0.2
@export var ascend_speed: float = 2.2

@export var ground_accel: float = 56.0
@export var air_accel: float = 48.0
@export var gravity: float = 32
@export var water_drag_accel: float = 0.20
@export var water_drag_speed: float = 0.45
@export var water_drag_jump = 0.35
@export var water_drag_gravity: float = 0.1

var held_item: ItemState
var health: int

var movement_enabled: bool = true
var in_air: bool = false
var under_water: bool = false
var buoyancy: float = 0.0

var speed_modifier: float = 1.0
var accel_modifier: float = 1.0
var jump_modifier: float = 1.0
var gravity_modifier: float = 1.0

signal hit_ground

func _physics_process(delta: float) -> void:
	if not is_on_floor():
		in_air = true
	if in_air and is_on_floor():
		in_air = false
		hit_ground.emit()
	velocity.y -= delta * gravity * gravity_modifier
	
	under_water = Ref.world.is_position_loaded(%WaterPoint.global_position) and Ref.world.is_under_water(%WaterPoint.global_position)
	
	if under_water:
		buoyancy = lerp(buoyancy, 1.0, delta * 16.0)
	else:
		buoyancy = lerp(buoyancy, 0.0, delta * 3.0)
	
	accel_modifier = lerp(1.0, water_drag_accel, buoyancy)
	jump_modifier = lerp(1.0, water_drag_jump, buoyancy)
	speed_modifier = lerp(1.0, water_drag_speed, buoyancy)
	gravity_modifier = lerp(1.0, water_drag_gravity, buoyancy)

func attack() -> void:
	pass
