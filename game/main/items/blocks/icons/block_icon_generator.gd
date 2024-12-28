class_name BlockIconGenerator extends SubViewportContainer

@export var icon_path: String = "res://main/items/blocks/icons/"
@export var block_path: String = "res://main/items/blocks/"

func _ready() -> void:
	visible = true
	await generate_icons(true)
	queue_free()

# Saving images is not really necessary;
# Icons are always generated when the game is started
func generate_icons(save_images: bool) -> void:
	var dir: DirAccess = DirAccess.open(block_path)
	dir.list_dir_begin()
	var file_name: String = dir.get_next()
	while file_name != "":
		if not dir.current_is_dir():
			var path: String = block_path + "/" + file_name
			if ResourceLoader.exists(path):
				var block: Block = ResourceLoader.load(path)
				%Block.get_surface_override_material(0).albedo_texture = block.texture
				
				await RenderingServer.frame_post_draw
				
				var capture: Image = %SubViewport.get_texture().get_image()
				block.icon = ImageTexture.create_from_image(capture)
				
				if save_images:
					capture.save_png(icon_path + "/" + block.display_name.to_snake_case() + ".png")
		file_name = dir.get_next()
