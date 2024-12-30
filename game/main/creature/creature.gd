class_name Creature extends CharacterBody3D

@export var dropped_item_scene: PackedScene

@export var max_health: int = 10
@export var pick_up_items: bool = true
@export var repickup_delay: float = 1.0

var movement_enabled: bool = true

var held_item: ItemState
var health: float

func _ready() -> void:
	%ItemArea3D.area_entered.connect(_on_dropped_item_entered)

func _on_dropped_item_entered(area: Area3D) -> void:
	if not area.get_parent() is DroppedItem:
		return
	var dropped_item: DroppedItem = area.get_parent() as DroppedItem
	if not dropped_item.can_collect:
		return
	
	var remaining_item: Item = %Inventory.accept(dropped_item.item)
	if remaining_item == null:
		dropped_item.collect()

func drop_item(item: ItemState, drop_position: Vector3, initial_velocity: Vector3) -> void:
	var new_item: DroppedItem = dropped_item_scene.instantiate()
	new_item.collect_delay_time = repickup_delay
	get_parent().add_child(new_item)
	new_item.initialize(item)
	new_item.global_position = drop_position
	new_item.velocity = initial_velocity
