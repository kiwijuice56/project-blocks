class_name MainWorld extends World
# An extension of the World class to more easily interface with GDscript
# Also sets block and decoration type arrays automatically

var x: int = 0

func _ready() -> void:
	reload_types()
	set_physics_process(false)
	initialize()
	# The noise texture is initially null while it is being generated
	if generator is DefaultGenerator:
		await generator.main_noise_texture.changed
	set_physics_process(true)

func _physics_process(_delta: float) -> void:
	set_loaded_region_center(%Player.global_position)
	if x % 4 == 0:
		print()
		print(%Player.global_position)
		print()
		
		simulate_water()
	x += 1

func _input(event: InputEvent) -> void:
	if generator is DebugGenerator and event.is_action_pressed("capture", false):
		capture_decoration()
	if event.is_action_pressed("hotbar_1"):
		simulate_water()

func sort_by_id(a: Block, b: Block) -> bool:
	return a.id < b.id

func reload_types() -> void:
	var stack: Array[String] = ["res://main/"]
	
	block_types.clear()
	decorations.clear()
	
	while stack.size() > 0:
		var dir: DirAccess = DirAccess.open(stack.pop_back())
		dir.list_dir_begin()
		var file_name: String = dir.get_next()
		while file_name != "":
			var path: String = dir.get_current_dir() + "/" + file_name
			if dir.dir_exists(path):
				stack.append(path)
			else:
				if ".tres" in path:
					path = path.replace(".remap", "")
					var resource: Resource = ResourceLoader.load(path)
					if resource is Block:
						block_types.append(resource as Block)
					if resource is Decoration:
						decorations.append(resource as Decoration)
			file_name = dir.get_next()
	
	block_types.sort_custom(sort_by_id)
 
# Helper method to easily build decorations within the game
func capture_decoration() -> void:
	var new_decoration: Decoration = Decoration.new()
	
	var min_corner: Vector3 = Vector3(16, 16, 16)
	var max_corner: Vector3 = Vector3()
	
	for y in range(0, 16):
		for z in range(0, 16):
			for x in range(0, 16):
				var block: Block = get_block_type_at(Vector3(x, y + 16, z))
				if block.id != 0:
					min_corner.x = min(x, min_corner.x)
					min_corner.y = min(y, min_corner.y)
					min_corner.z = min(z, min_corner.z)
					max_corner.x = max(x, max_corner.x)
					max_corner.y = max(y, max_corner.y)
					max_corner.z = max(z, max_corner.z)
	
	max_corner += Vector3(1, 1, 1)
	
	var blocks: PackedInt32Array = PackedInt32Array()
	
	new_decoration.size = max_corner - min_corner
	blocks.resize(int(new_decoration.size.x * new_decoration.size.y * new_decoration.size.z))

	for y in range(min_corner.y, max_corner.y):
		for z in range(min_corner.z, max_corner.z):
			for x in range(min_corner.x, max_corner.x):
				var block: Block = get_block_type_at(Vector3(x, y + 16, z))
				var i: int = int((x - min_corner.x) + new_decoration.size.x * (z - min_corner.z) + new_decoration.size.x * new_decoration.size.z * (y - min_corner.y))
				blocks[i] = block.id
	
	new_decoration.blocks = blocks
	
	var id: int = randi_range(10000, 90000)
	print("resource %d saved" % id)
	ResourceSaver.save(new_decoration, "res://main/world/decorations/new_deco%d.tres" % id)
