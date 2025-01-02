class_name Camera extends Camera3D

signal under_water_status_changed(under_water: bool)

var under_water: bool = false:
	set(val):
		if val != under_water:
			under_water_status_changed.emit(val)
		under_water = val

func _process(_delta: float) -> void:
	under_water = Ref.world.is_position_loaded(global_position) and Ref.world.get_water_level_at(global_position) > 0
	set_cull_mask_value(2, not under_water)
