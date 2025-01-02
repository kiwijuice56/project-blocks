class_name MainEnvironment extends WorldEnvironment

@export var water_fog_density: float = 0.6
@export var water_fog_light_color: Color
@export var water_antriscopy: float

var default_fog_density: float
var default_fog_light_color: Color
var default_antriscopy: float

func _ready() -> void:
	Ref.player_camera.under_water_status_changed.connect(_on_under_water_status_changed)
	default_fog_light_color = environment.fog_light_color
	default_fog_density = environment.fog_density
	default_antriscopy = environment.volumetric_fog_anisotropy

func _on_under_water_status_changed(new_val: bool) -> void:
	if new_val:
		environment.fog_density = water_fog_density
		environment.fog_light_color = water_fog_light_color
		environment.volumetric_fog_albedo = water_fog_light_color
		environment.volumetric_fog_anisotropy = water_antriscopy
	else:
		environment.fog_density = default_fog_density
		environment.fog_light_color = default_fog_light_color
		environment.volumetric_fog_albedo = Color.WHITE
		environment.volumetric_fog_anisotropy = default_antriscopy
