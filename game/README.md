# project-blocks
A voxel game created in Godot 4.4 using GDExtension.

## Structure
### Entities
Entities are the base class for most dynamic and interactable objects, such as the player and other creatures. `Entity` scripts act as the "glue" for the various components that make up a specific entity, such as its stats, mesh instances, or collision shapes. The majority of logic for an entity is handled via composition using `Behavior` scripts, each which implement specific behaviors like `PickUpItems` or `DropItems`.

### Items
Static information about items (such as the display name and icon) is stored using `Item` resources. An `Item` resource exists for every type of item in the game, such as blocks and tools. Dynamic information is stored in `ItemState` resources, which stores the `id` of a particular `Item` along with dynamic information such as the number of blocks in a stack or the durability of a tool. The `Item` resource belonging to an `id` can be quickly accessed using the `ItemMap` autoload script. Finally, the behavior of an item when it is held by an entity is coded using `HeldItem` scenes. `HeldItem` scenes are not necessarily unique to a specific `Item`.

For a quick example of how items work in-game, consider the example of the player switching their held item to a sword in the hotbar: The player stores their items in an array of `ItemState` resources. The icon, name, and `HeldItem` scene of the sword are acquired by finding the `Item` resource whose `id` matches the `ItemState` of the currently selected item. Next, the `HeldItem` scene is instantiated and placed at the player's hand, where it is initialized according to the information in the `ItemState`. Once initialized, the `HeldItem` increases the attack stat of the player temporarily.

### Blocks and Water
Entities, behaviors, and held items can interact with the environment via the `World` singleton (accessed using `Ref.world` in GDscript). The world class contains the state of all blocks and water. The majority of complex logic is handled by `World`, so scripts can rely on simple methods such as `break_block_at` and `place_water_at` as long as any queried positions are loaded (verified using `is_position_loaded`).