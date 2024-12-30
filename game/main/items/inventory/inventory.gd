class_name Inventory extends Node
# Collection of items

@export var capacity: int

var items: Array[Item]

# Emitted when this inventory requires a UI update.. items picked up,
# used, etc.
signal refresh

func _ready() -> void:
	items.resize(capacity)

func change_amount(index: int, amount: int) -> void:
	items[index].count += amount
	items[index].count = min(items[index].count, items[index].stack_size)
	if items[index].count <= 0:
		items[index] = null
	refresh.emit()

# Attempts to pack an item into this inventory, returning the remaining items
# if there is not enough space
func accept(item: Item) -> Item:
	var changed: bool = false
	
	# First, try putting the item into a matching stack
	for i in range(capacity):
		if items[i] == null:
			continue
		
		if items[i].id == item.id:
			changed = true
			
			var total_count: int = items[i].count + item.count
			items[i].count = min(items[i].stack_size, total_count)
			item.count = total_count - items[i].count
			
			# The item was fully consumed
			if item.count == 0:
				refresh.emit()
				return null
	
	# Then, attempt to put it  into the first empty slot
	for i in range(capacity):
		if items[i] == null:
			items[i] = item
			refresh.emit()
			return null
	# Give up :(
	if changed:
		refresh.emit()
	return item
