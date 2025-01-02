class_name Inventory extends Node
# Collection of items

@export var capacity: int

var items: Array[ItemState]

# Emitted when this inventory requires a UI update.. items picked up,
# used, etc.
signal refresh(index: int)

func _ready() -> void:
	items.resize(capacity)
	items[0] = ItemState.new()
	items[0].id = 2
	items[0].count = 64

func change_amount(index: int, amount: int) -> void:
	items[index].count += amount
	items[index].count = min(items[index].count, ItemMap.map(items[index].id).stack_size)
	if items[index].count <= 0:
		items[index] = null
	refresh.emit(index)

# Attempts to pack an item into this inventory, returning the remaining items
# if there is not enough space
func accept(item: ItemState) -> ItemState:
	# First, try putting the item into a matching stack
	for i in range(capacity):
		if items[i] == null:
			continue
		
		if items[i].id == item.id:
			var initial_amount: int = items[i].count
			
			var total_count: int = items[i].count + item.count
			items[i].count = min(ItemMap.map(items[i].id).stack_size, total_count)
			item.count = total_count - items[i].count
			
			if items[i].count != initial_amount:
				refresh.emit(i)
			
			# The item was fully consumed
			if item.count == 0:
				return null
	# Then, attempt to put it  into the first empty slot
	for i in range(capacity):
		if items[i] == null:
			items[i] = item
			refresh.emit(i)
			return null
	return item
