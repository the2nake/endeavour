# Saving and Loading

## Things that need to be saved
 * How the environment has been changed by the player: what chests contain, flicked levers, etc.
 * NPCs: data, location
 * Player: quests, inventory, location

## Structure

Each save is contained within a folder. Each folder contains:

 * One file for each level, storing the map, which entities are in it, chests, etc.
   * This includes any NPCs in the level, which are loaded into the game when the player enters the level
   * The actual data for NPCs are contained in other files. level files only contain references to NPCs. Entities such as chests which are not marked as NPCs (and thus will not traverse between levels) have their data stored in level files
 * NPCs have their own files with the exact same layout as the player's, but do not include quest data
 * One player file, including the player's inventory, quests, and which level they are in.
   * The first level that includes the player is loaded and shown in the game

## Before gameplay

The level data that contains the player will be loaded

### Repeating until the game ends:

1. The game loads levels adjacent to the current one, based on the level's save data, which contains special tiles which point to exit locations in other levels
2. When the player enters an adjacent level, all adjacent levels are removed from memory and a new list of adjacent levels are loaded based on the new level's exit points
   * Note that levels may contain one-way exits: some tiles (such as pit tiles) lead to an exit tile which doesn't lead back.
   * The check to go to another level is decided in the movement update code, and thus should _not_ be triggered by level entry.
3. Levels adjacent to the current level are simulated as well

### Implementation of level

#### Tiles

- Includes all the tiles, their source files, and various attributes.
  - These are movementCost, cropRect, outRect, isNatural, collisionRect, collisionRect2, and name
    - The collisionRect is a relative rectangle based on the top left of the tile
      - This attribute only applies to tiles with negative movementCost
      - collisionRect2 can be used for compound boxes

#### Layers

- There are overlay and background layers
- Background layers are drawn before the player
- Foreground layers are drawn after the player
- **only** the bottom background layer's movementCost matters

- TODO: Implement overlay handling
  - Separate from the normal foreground/background
  - Not affected by screen scrolling
  - Handlers for mouse/keyboard events?

#### NPCs

- This does not include the player
  