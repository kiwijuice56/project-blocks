class_name Creature extends CharacterBody3D

@export var max_health: int = 10

var movement_enabled: bool = true

var held_item: Item
var health: float

func _ready() -> void:
	%ItemArea3D.area_entered.connect(_on_dropped_item_entered)

func _on_dropped_item_entered(area: Area3D) -> void:
	if not area.get_parent() is DroppedItem:
		return
	var dropped_item: DroppedItem = area.get_parent() as DroppedItem
	if dropped_item.state == DroppedItem.COLLECTED:
		return
	
	var remaining_item: Item = %Inventory.accept(dropped_item.item)
	if remaining_item == null:
		dropped_item.collect()
