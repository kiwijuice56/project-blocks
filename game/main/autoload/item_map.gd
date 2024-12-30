extends Node

@export var item_path: String = "res://main/items/"

var id_to_resource: Dictionary[int, Item]

func _ready() -> void:
	var stack: Array[String] = [item_path]
	
	while stack.size() > 0:
		var dir: DirAccess = DirAccess.open(stack.pop_back())
		dir.list_dir_begin()
		var file_name: String = dir.get_next()
		while file_name != "":
			var path: String = dir.get_current_dir() + "/" + file_name
			if dir.dir_exists(path):
				stack.append(path)
			else:
				if ResourceLoader.exists(path):
					var resource: Resource = ResourceLoader.load(path)
					if resource is Item:
						var item: Item = resource as Item 
						id_to_resource[item.id] = item
			file_name = dir.get_next()

func map(id: int) -> Item:
	return id_to_resource[id]
