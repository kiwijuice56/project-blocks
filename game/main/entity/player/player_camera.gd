class_name Camera extends Camera3D

signal under_water_status_changed(under_water: bool)

@export var max_bubble_volume: float = -3.0
@export var min_bubble_volume: float = -60

var low_pass: AudioEffectLowPassFilter = AudioServer.get_bus_effect(2, 0)

var under_water: bool = false:
	set(val):
		# The camera makes a splashing sound when it exits the water so
		# there isn't a jarring silence when your head pops up -- 
		# unique from other entities
		if under_water and not val:
			%Splash.splash(Ref.player.velocity.y)
		if val != under_water:
			under_water_status_changed.emit(val)
		under_water = val

func _process(delta: float) -> void:
	under_water = Ref.world.is_position_loaded(global_position) and Ref.world.is_under_water(global_position)
	set_cull_mask_value(2, not under_water)
	set_cull_mask_value(3, under_water)
	low_pass.cutoff_hz = lerp(low_pass.cutoff_hz, 600. if under_water else 20500., delta * 4.0) 
	%BubblesPlayer.volume_db = lerp(%BubblesPlayer.volume_db , max_bubble_volume if under_water else min_bubble_volume, delta * 2.0)
