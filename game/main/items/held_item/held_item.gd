class_name HeldItem extends Node3D

var item: Item
var holder: Entity

func initialize(set_item: Item, set_holder: Entity) -> void:
	self.item = set_item
	self.holder = set_holder

func on_hold() -> void:
	pass

func on_unhold() -> void:
	pass

func interact() -> void:
	pass
