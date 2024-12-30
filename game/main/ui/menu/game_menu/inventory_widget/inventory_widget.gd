class_name InventoryWidget extends GridContainer
# A visual representation of an inventory

@export var inventory_slot_scene: PackedScene

var inventory: Inventory

func _ready() -> void:
	reset()

func _on_item_changed(slot: InventorySlot):
	inventory.items[slot.index] = slot.item

func _on_refresh(index: int) -> void:
	get_child(index).initialize(inventory.items[index], index)

func initialize(new_inventory: Inventory) -> void:
	inventory = new_inventory
	if not inventory.refresh.is_connected(_on_refresh):
		inventory.refresh.connect(_on_refresh)
	
	if get_child_count() != inventory.capacity:
		for child in get_children():
			remove_child(child)
			child.queue_free()
		
		for i in range(inventory.capacity):
			var new_slot: InventorySlot = inventory_slot_scene.instantiate()
			new_slot.item_changed.connect(_on_item_changed)
			add_child(new_slot)
	
	for i in range(get_child_count()):
		get_child(i).initialize(inventory.items[i], i)
	
	get_viewport().update_mouse_cursor_state.call_deferred()

func reset() -> void:
	if is_instance_valid(inventory):
		inventory.refresh.disconnect(_on_refresh)
	inventory = null
	for child in get_children():
		child.queue_free()
		remove_child(child)
