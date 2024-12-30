class_name GameMenu extends Menu

enum { DEFAULT, INVENTORY }

var state: int = DEFAULT

func _ready() -> void:
	%PlayerHotbar.initialize(Ref.player_hotbar)
	close_inventory()

func _input(event: InputEvent) -> void:
	if state == DEFAULT and event.is_action_pressed("inventory", false):
		open_inventory()
	elif state == INVENTORY and event.is_action_pressed("inventory", false):
		close_inventory()

func open_inventory() -> void:
	%Crosshair.visible = false
	state = INVENTORY
	%Inventory.initialize(Ref.player_inventory)
	%Inventory.visible = true
	%Cover.visible = true
	
	Ref.player.movement_enabled = false
	
	Input.mouse_mode = Input.MOUSE_MODE_VISIBLE

func close_inventory() -> void:
	# Take care of any held items
	if InventorySlot.state == InventorySlot.HOLDING_ITEM:
		var held_item: Item = InventorySlot.held_item.item
		InventorySlot.held_item.queue_free()
		
		var remaining_item: Item = Ref.player_hotbar.accept(held_item)
		if remaining_item != null:
			remaining_item = Ref.player_inventory.accept(held_item)
		if remaining_item != null:
			pass # todo: force drop item
		
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
	
	Ref.player.movement_enabled = true
	
	Input.mouse_mode = Input.MOUSE_MODE_CAPTURED
