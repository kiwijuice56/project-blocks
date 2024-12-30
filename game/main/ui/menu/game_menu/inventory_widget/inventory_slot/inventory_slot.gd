class_name InventorySlot extends TextureRect
# A single slot of an InventoryWidget

@export var normal_texture: Texture
@export var hover_texture: Texture
@export var item_widget_scene: PackedScene

var item_widget: ItemWidget
var item: Item
var index: int = 0

signal item_changed(slot: InventorySlot)

# All dragging/dropping logic is handled within the static class, since there
# can only be one dragged item at a time
enum { IDLE, HOLDING_ITEM }

static var state: int = IDLE
static var held_item: ItemWidget

static func pick_up(slot: InventorySlot) -> void:
	state = HOLDING_ITEM
	
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

static func deposit(slot: InventorySlot) -> void:
	# Mismatched items (skip over and do nothing)
	if slot.item != null and slot.item.id != held_item.item.id:
		return
	# Full target (skip over and do nothing)
	elif slot.item != null and slot.item.count >= slot.item.stack_size:
		return
	# Only one block (just drop instead)
	elif held_item.item.count == 1:
		drop(slot)
	# Complete transfer to empty slot (creates new item)
	elif slot.item == null:
		held_item.item.count -= 1
		held_item.initialize(held_item.item)
		
		var new_item: Item = held_item.item.duplicate()
		new_item.count = 1
		slot.initialize(new_item)
		slot.item_changed.emit(slot)
	# Just dropping an item into a bigger stack
	else:
		held_item.item.count -= 1
		held_item.initialize(held_item.item)
		
		slot.item.count += 1
		slot.initialize(slot.item)
		slot.item_changed.emit(slot)

var hovered_over: bool = false

func _ready() -> void:
	mouse_entered.connect(_on_mouse_entered)
	mouse_exited.connect(_on_mouse_exited)

func _on_mouse_entered() -> void:
	hovered_over = true
	texture = hover_texture

func _on_mouse_exited() -> void:
	hovered_over = false
	texture = normal_texture

func _input(event: InputEvent) -> void:
	if event.is_action_pressed("select", false) and hovered_over and state == IDLE and item_widget.item != null:
		pick_up(self)
	elif event.is_action_pressed("select", false) and hovered_over and state == HOLDING_ITEM:
		drop(self)
	elif event.is_action_pressed("deposit") and hovered_over and state == HOLDING_ITEM:
		deposit(self)

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
