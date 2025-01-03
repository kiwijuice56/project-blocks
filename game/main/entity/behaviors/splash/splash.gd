class_name Splash extends Behavior

@export var splash_player_scene: PackedScene = preload("res://main/entity/behaviors/splash/splash_player.tscn")
@export var splash_effect_scene: PackedScene = preload("res://main/entity/behaviors/splash/splash_effect.tscn")
@export var small_splash: AudioStream = preload("res://main/entity/behaviors/splash/small_splash.tres")
@export var big_splash: AudioStream = preload("res://main/entity/behaviors/splash/big_splash.tres")

@export var big_threshold: float = 8.0
@export var enter_splash: bool = true
@export var exit_splash: bool = true

func _ready() -> void:
	super._ready()
	entity.water_entered.connect(_on_hit_water)
	entity.water_exited.connect(_on_water_exited)

func _on_hit_water(vertical_velocity: float) -> void:
	if enter_splash:
		splash(vertical_velocity)

func _on_water_exited(vertical_velocity: float) -> void:
	if exit_splash:
		splash(vertical_velocity)

func splash(vertical_velocity: float, particles: bool = true) -> void:
	var new_player: AudioStreamPlayer3D = splash_player_scene.instantiate()
	get_tree().get_root().add_child(new_player)
	if abs(vertical_velocity) > big_threshold:
		new_player.stream = big_splash
	else:
		new_player.stream = small_splash
	new_player.global_position = entity.global_position
	new_player.playing = true
	new_player.finished.connect(new_player.queue_free)
	if particles and abs(vertical_velocity) > big_threshold:
		var new_effect: GPUParticles3D = splash_effect_scene.instantiate()
		get_tree().get_root().add_child(new_effect)
		new_effect.global_position = entity.global_position
		new_effect.emitting = true
