class_name Behavior extends Node3D

@export var enabled: bool = true
@export var entity: Entity

func _ready() -> void:
	assert(is_instance_valid(entity))
