class_name PickUpItems extends Behavior

@export var inventory_priority: Inventory
@export var inventory_secondary: Inventory
@export var dropped_item_scene: PackedScene = preload("res://main/items/dropped_item/dropped_item.tscn")
@export var shape: Shape3D

func _ready() -> void:
	super._ready()
	%ItemArea3D.area_entered.connect(_on_dropped_item_entered)
	%CollisionShape3D.shape = shape
	assert(is_instance_valid(inventory_priority))

func _on_dropped_item_entered(area: Area3D) -> void:
	if not enabled:
		return
	if not area.get_parent() is DroppedItem:
		return
	var dropped_item: DroppedItem = area.get_parent() as DroppedItem
	if not dropped_item.can_collect:
		return
	var remaining_item: ItemState = inventory_priority.accept(dropped_item.item)
	if remaining_item == null:
		dropped_item.collect()
	elif is_instance_valid(inventory_secondary):
		remaining_item = inventory_secondary.accept(dropped_item.item)
		if remaining_item == null:
			dropped_item.collect()
