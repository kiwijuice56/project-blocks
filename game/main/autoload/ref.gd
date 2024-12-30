extends Node

@onready var main: Main = get_tree().get_root().get_node("Main")
@onready var player: Player = main.get_node("Player")
@onready var ui: CanvasLayer = main.get_node("UI")

@onready var player_inventory: Inventory = main.get_node("Inventories/PlayerInventory")
@onready var player_hotbar: Inventory = main.get_node("Inventories/PlayerHotbar")
