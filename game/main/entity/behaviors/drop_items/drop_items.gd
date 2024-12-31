class_name DropItems extends Behavior

@export var dropped_item_scene: PackedScene = preload("res://main/items/dropped_item/dropped_item.tscn")
@export var collect_delay_time: float = 1.0
@export var target_raycast: RayCast3D
@export var drop_spawn: Marker3D
@export var throw_speed: float = 16.0

func _ready() -> void:
	super._ready()
	assert(is_instance_valid(target_raycast))

func drop_and_remove_from_inventory(inventory: Inventory, index: int) -> void:
	var to_drop: ItemState = inventory.items[index]
	if to_drop != null:
		to_drop = to_drop.duplicate()
		to_drop.count = 1
		inventory.change_amount(index, -1)
		drop_item(to_drop)

func drop_item(item: ItemState) -> void:
	%PopPlayer.play()
	var new_item: DroppedItem = dropped_item_scene.instantiate()
	new_item.collect_delay_time = collect_delay_time
	get_tree().get_root().add_child(new_item)
	new_item.global_position = drop_spawn.global_position - Vector3(0.5, 0.5, 0.5)
	new_item.initialize(item)
	if new_item.state != DroppedItem.SWIM:
		new_item.velocity = get_throw_direction() * throw_speed + entity.velocity

func get_throw_direction() -> Vector3:
	if target_raycast.is_colliding():
		var look_position: Vector3 = target_raycast.get_collision_point()
		return (look_position - target_raycast.global_position).normalized()
	else:
		return (target_raycast.to_global(target_raycast.target_position) - drop_spawn.global_position).normalized()
