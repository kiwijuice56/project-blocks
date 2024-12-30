class_name ItemWidget extends Control

var item: ItemState
var follow_mouse_offset: Vector2
var follow_mouse: bool = false:
	set(val):
		follow_mouse_offset = -Vector2(9.5, 9.5)
		follow_mouse = val
		set_process(follow_mouse)

func _ready() -> void:
	set_process(false)

func _process(_delta: float) -> void:
	global_position = get_global_mouse_position() + follow_mouse_offset

func initialize(new_item: ItemState) -> void:
	item = new_item
	
	if item == null:
		visible = false
		return
	
	%Icon.texture = ItemMap.map(item.id).icon
	%Label.visible = ItemMap.map(item.id).stack_size != 1 
	%Label.text = str(item.count)
