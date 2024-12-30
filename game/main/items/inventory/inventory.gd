class_name Inventory extends Node

@export var capacity: int

var items: Array[Item]

func _ready() -> void:
	items.resize(capacity)
	
	items[0] = preload("res://main/items/blocks/dirt.tres").duplicate()
	items[0].count = 4
	
	items[2] = preload("res://main/items/blocks/stone.tres").duplicate()
	items[2].count = 12
	
	items[3] = preload("res://main/items/blocks/wood.tres").duplicate()
	items[3].count = 5
	
	items[6] = preload("res://main/items/blocks/dirt.tres").duplicate()
	items[6].count = 48
