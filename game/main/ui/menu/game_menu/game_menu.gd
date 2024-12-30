class_name GameMenu extends Menu

enum { DEFAULT, INVENTORY }

var state: int = DEFAULT

func _ready() -> void:
	close_inventory()

func _input(event: InputEvent) -> void:
	if state == DEFAULT and event.is_action_pressed("inventory", false):
		open_inventory()
	elif state == INVENTORY and event.is_action_pressed("inventory", false):
		close_inventory()

func open_inventory() -> void:
	%Crosshair.visible = false
	state = INVENTORY
	%Inventory.open(Ref.player_inventory)
	%Inventory.visible = true
	%Cover.visible = true
	
	Ref.player.movement_enabled = false
	
	Input.mouse_mode = Input.MOUSE_MODE_VISIBLE

func close_inventory() -> void:
	%Crosshair.visible = true
	state = DEFAULT
	%Inventory.close()
	%Inventory.visible = false
	%Cover.visible = false
	
	Ref.player.movement_enabled = true
	
	Input.mouse_mode = Input.MOUSE_MODE_CAPTURED
