class_name Inventory extends Node

@export var capacity: int
@export var inventory_widget: InventoryWidget

var items: Array[Item]

func _ready() -> void:
	items.resize(capacity)
	
	items[0] = preload("res://main/items/blocks/dirt.tres")
	items[0].count = 4
	
	items[2] = preload("res://main/items/blocks/glass.tres")
	items[2].count = 12
	
	items[3] = preload("res://main/items/blocks/wood.tres")
	items[3].count = 5
	
	inventory_widget.open(self)
