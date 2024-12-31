class_name Entity extends CharacterBody3D

@export_group("Stats")
@export var max_health: int = 10
@export var strength: int

var held_item: ItemState
var health: int
var movement_enabled: bool = true
var in_air: bool = false

signal hit_ground

func _physics_process(_delta: float) -> void:
	if not is_on_floor():
		in_air = true
	if in_air and is_on_floor():
		in_air = false
		hit_ground.emit()

func attack() -> void:
	pass
