class_name InventoryWidget extends GridContainer

@export var inventory_slot_scene: PackedScene

var inventory: Inventory

func _on_item_changed(slot: InventorySlot):
	inventory.items[slot.index] = slot.item

func open(new_inventory: Inventory) -> void:
	inventory = new_inventory
	
	for child in get_children():
		child.queue_free()
		remove_child(child)
	for i in range(inventory.capacity):
		var new_slot: InventorySlot = inventory_slot_scene.instantiate()
		new_slot.initialize(inventory.items[i], i)
		new_slot.item_changed.connect(_on_item_changed)
		add_child(new_slot)

func close() -> void:
	for child in get_children():
		child.queue_free()
		remove_child(child)
