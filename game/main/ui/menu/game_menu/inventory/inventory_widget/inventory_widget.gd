class_name InventoryWidget extends GridContainer

@export var inventory_slot_scene: PackedScene

var inventory: Inventory

func _on_item_changed(slot: InventorySlot):
	inventory.items[slot.index] = slot.item

func _on_refresh() -> void:
	initialize(inventory)

func initialize(new_inventory: Inventory) -> void:
	inventory = new_inventory
	if not inventory.refresh.is_connected(_on_refresh):
		inventory.refresh.connect(_on_refresh)
	
	for child in get_children():
		child.queue_free()
		remove_child(child)
	
	for i in range(inventory.capacity):
		var new_slot: InventorySlot = inventory_slot_scene.instantiate()
		new_slot.initialize(inventory.items[i], i)
		new_slot.item_changed.connect(_on_item_changed)
		add_child(new_slot)

func reset() -> void:
	if is_instance_valid(inventory):
		inventory.refresh.disconnect(_on_refresh)
	inventory = null
	for child in get_children():
		child.queue_free()
		remove_child(child)
