extends Node

@onready var main: Main = get_tree().get_root().get_node("Main")
@onready var world: World = main.get_node("MainWorld")
@onready var player: Player = main.get_node("Player")
@onready var ui: CanvasLayer = main.get_node("UI")

@onready var player_inventory: Inventory = player.get_node("%Inventory")
@onready var player_hotbar: Inventory = player.get_node("%Hotbar")
