class_name GameMenu extends Menu
# Handles the main screen seen during gameplay, such as the inventory and hotbar

enum { DEFAULT, INVENTORY }

var state: int = DEFAULT

func _ready() -> void:
	%PlayerHotbar.initialize(Ref.player_hotbar)
	Ref.player.hotbar_index_changed.connect(_on_hotbar_index_changed)
	close_inventory()

func _input(event: InputEvent) -> void:
	if state == DEFAULT and event.is_action_pressed("inventory", false):
		open_inventory()
	elif state == INVENTORY and event.is_action_pressed("inventory", false):
		close_inventory()
	
	if state == INVENTORY and event.is_action_pressed("drop_item", false):
		if InventorySlot.held_item != null:
			Ref.player.throw_item(InventorySlot.held_item.item)
			InventorySlot.held_item.queue_free()
			InventorySlot.held_item = null
			InventorySlot.state = DEFAULT
		elif InventorySlot.hovered_slot != null:
			Ref.player.drop_from_inventory(InventorySlot.hovered_slot.get_parent().inventory, InventorySlot.hovered_slot.index)

func _on_hotbar_index_changed() -> void:
	# Offset the outline image
	%HotbarOutline.position.x = 22 * Ref.player.hotbar_index

func open_inventory() -> void:
	%Crosshair.visible = false
	state = INVENTORY
	%Inventory.initialize(Ref.player_inventory)
	%Inventory.visible = true
	%Cover.visible = true
	
	%HotbarOutline.visible = false
	
	Ref.player.movement_enabled = false
	Ref.player.can_switch_hotbar = false
	
	Input.mouse_mode = Input.MOUSE_MODE_VISIBLE

func close_inventory() -> void:
	if InventorySlot.hovered_slot != null:
		InventorySlot.hovered_slot._on_mouse_exited()
	# Take care of any held items
	if InventorySlot.state == InventorySlot.HOLDING_ITEM:
		var held_item: Item = InventorySlot.held_item.item
		InventorySlot.held_item.queue_free()
		
		var remaining_item: Item = Ref.player_hotbar.accept(held_item)
		if remaining_item != null:
			remaining_item = Ref.player_inventory.accept(remaining_item)
		if remaining_item != null:
			Ref.player.throw_item(remaining_item)
		
		# We don't also have to re-initialize the inventory here
		# because it is being hidden anyways
		%PlayerHotbar.initialize(Ref.player_hotbar)
		
		InventorySlot.state = InventorySlot.IDLE
		InventorySlot.held_item = null
	
	%Crosshair.visible = true
	state = DEFAULT
	%Inventory.reset()
	%Inventory.visible = false
	%Cover.visible = false
	
	%HotbarOutline.visible = true
	
	Ref.player.movement_enabled = true
	Ref.player.can_switch_hotbar = true
	
	Input.mouse_mode = Input.MOUSE_MODE_CAPTURED
