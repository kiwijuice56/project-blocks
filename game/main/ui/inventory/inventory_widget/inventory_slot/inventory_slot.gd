class_name InventorySlot extends TextureRect

@export var item_widget_scene: PackedScene

var item_widget: ItemWidget

var item: Item
var index: int = 0

signal item_changed(slot: InventorySlot)

enum { IDLE, HOLDING_ITEM, }

static var state: int = IDLE
static var source_slot: InventorySlot
static var held_item: ItemWidget
static var any_hover: bool = false

static func pick_up(slot: InventorySlot) -> void:
	if state != IDLE:
		return
	state = HOLDING_ITEM
	
	source_slot = slot
	
	var selected_item: Item = slot.item
	
	slot.initialize(null)
	slot.item_changed.emit(slot)
	
	held_item = slot.item_widget_scene.instantiate()
	held_item.initialize(selected_item)
	Ref.ui.add_child(held_item)
	
	held_item.global_position = slot.global_position
	held_item.follow_mouse = true

static func drop(slot: InventorySlot) -> void:
	# Mismatched items (swap)
	if slot.item != null and slot.item.id != held_item.item.id:
		var to_swap: Item = slot.item
		
		slot.initialize(held_item.item)
		slot.item_changed.emit(slot)
		
		held_item.initialize(to_swap)
		
		return
	# Complete transfer to empty slot
	elif slot.item == null:
		slot.initialize(held_item.item)
		slot.item_changed.emit(slot)
	# Drop some and hold remaining
	elif slot.item.id == held_item.item.id:
		var total_count: int = held_item.item.count + slot.item.count
		slot.item.count = min(total_count, slot.item.stack_size)
		held_item.item.count = total_count - slot.item.count
		
		slot.initialize(slot.item)
		slot.item_changed.emit(slot)
		
		if held_item.item.count > 0:
			held_item.initialize(held_item.item)
			return
	
	held_item.queue_free()
	state = IDLE

var hovered_over: bool = false

func _ready() -> void:
	mouse_entered.connect(_on_mouse_entered)
	mouse_exited.connect(_on_mouse_exited)

func _on_mouse_entered() -> void:
	any_hover = true
	hovered_over = true

func _on_mouse_exited() -> void:
	any_hover = false
	hovered_over = false

func _input(event: InputEvent) -> void:
	if event.is_action_pressed("select", false) and hovered_over and state == IDLE and item_widget.item != null:
		pick_up(self)
	elif event.is_action_pressed("select", false) and hovered_over and state == HOLDING_ITEM:
		drop(self)

func initialize(new_item: Item, new_index: int = -1) -> void:
	item = new_item
	
	if new_index >= 0:
		index = new_index
	
	if is_instance_valid(item_widget):
		item_widget.queue_free()
	
	var new_widget: ItemWidget = item_widget_scene.instantiate()
	new_widget.initialize(item)
	add_child(new_widget)
	
	item_widget = new_widget
