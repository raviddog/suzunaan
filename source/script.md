# untitled engine scripting reference

slightly less shit documentation for touhou suzunaan's yet to be named scripting language

## Contents

- [Script types](#script-types)
- [Script loader](#script-loader-script)
- [Stage script](#stage-script)
  - [Instructions](#stage-instruction-list)
- [Enemy script](#enemy-script)
  - [Triggers](#enemy-triggers)
  - [Instructions](#enemy-instruction-list)
- [Bullet script](#bullet-script)
  - [Triggers](#bullet-triggers)
  - [Instructions](#bullet-instruction-list)

## Script types

There are 4 script types.

- Scriptloader
  - contains filenames of the scripts to load for the stage, and assigns IDs to each script
- Stage
  - contains the spawn instructions for enemies and optionally bullets within the stage. It only has frame triggers.
- Enemy
  - behaviour instructions for enemies and other non-bullet entities such as spawners and stuff
- Bullet
  - behaviour instructions for bullets

All files begin with a script type specifier. This is mostly just for idiot protection + organisation, there's no actual real error checking or anything so idk.

```
type: bullet
```

The types are `scriptloader`, `stage`, `bullet`, `enemy`. These tell the parser (and human) what type of script the file is. The rest of the file contains the instructions for that particular script.


## Script loader script

After the script type specifier, the scriptloader file is split into 3 sections, `#stage`, `#enemy` and `#bullet`, for each 3 types of scripts. These don't need to be in this order, and in fact don't need to grouped either, you may have multiple sections of each. A line containing a section header simply informs the parser to treat the next scripts as that particular script type until it is changed.

The stage section should only contain 1 script, and it should not be given an ID. Simply type the file path of the script on its own line.

The other script lines are laid out like this:

`[ID]:[path-to-script.txt]`

The ID is an integer greater than 0. IDs are unique to each section, and are what are referenced by all spawn instructions in all scripts. The script should match the currently declared script type, or it will be ignored.

An example scriptloader looks like this.

```
type: scriploader

#stage
teststage.txt

#enemy
1:testenemy.txt

#bullet
1:b.txt
```

## Stage script

The stage script's only job is to spawn in entities with scripts attached. It acts upon a frame counter. Each line contains the frame it should be triggered on, and the list of instructions that should be executed on that particular frame. Instructions for a particular frame must all be on a single line, separated by spaces, with the line terminated with a semicolon `;`.

The script lines are laid out like this:

`[frame]:[instruction1(arguments)] [instruction2(arguments)];`

I think multiple lines can contain the same frame and it shouldn't crash.

An example stage script looks like this.

```
type: scriploader

#stage
teststage.txt

#enemy
1:testenemy.txt
2:testenemy2.txt

#bullet
1:b.txt
2:b2.txt

```

### Stage Instruction list

- enemy(int type, int scriptID, float x, float y)
  - Spawns an enemy with given type and attached enemy script, at location (x, y)
  - The type determines the sprite used, hitbox size, [TODO] and the starting HP

## Enemy script

Enemy scripts contain behaviour and spawn instructions for most non-bullet entities.

The script lines are laid out like this:

`[ID]:[trigger(arguments)]:[instruction1(arguments)] [instruction2(arguments)]`

Each script line is split into three parts: an (optional) instruction ID, a trigger that determines when the 
instruction is called, and the actual instructions to be executed. These sections are separated with a colon `:`.

The instruction ID is used to refer to this set of instructions, and each instruction set can have unlimited instructions.

### Enemy Triggers

- init()
  - a trigger that is called just before the enemy is activated
  - you can use it to set things like intial hp, speed, etc

- death()
  - trigger called when hp reaches 0

- none()
  - trigger by ID only
  - single run instruction

- noneCont()
  - trigger by ID only
  - runs until stopped by ID

- frame(int frame)
  - runs on specified frame
  - single run instruction

- frameCont(int frame)
  - runs every frame starting at specified frame
  - runs until stopped by ID

- interval(int frames)
  - run every x frames
  - start with ID

- intervalStart(int start, int frames)
  - starting from specified frame, run every x frames
  - cancel with ID

### Enemy Instruction list

- speed (float speed)
  - sets the speed to the specified value

- angle (float angle)
  - sets the movement/facing direction to the specified value (degrees, 0 - 360). 0 degrees is up

- accel (float accel)
  - sets the acceleration

- bullet (int type, int scriptID, float speed, float angle)
  - spawns a bullet
    - type is bullet type
    - scriptID is the ID of the script to be attached to the bullet.

- setHP (int hp)
  - sets the unit's hp

- stop (int ID)
  - stops the instruction with given ID

- start (int ID)
  - runs the instruction with given ID

- frameTrigger (int ID, int frame)
  - creates a new frame trigger for specified instruction id
  - mostly used internally by functions that stop automatically

- frameTriggerOffset (int ID, int frame)
  - same as frameTrigger, but frame is an offset to the current instruction frame
  - used internally by the interval triggers

- stopInterval (int ID)
  - same as stop but specifically for interval functions

- enemy (int type, int scriptID, float X, float Y)
  - spawn enemy

- angle_change (float angle)
  - add the specified amount to the current angle
  - angles outside the range 0 - 360 will wrap around so its fine

- bullet_ring (int type, int scriptID, float speed, int ringNum)
  - fires a ring of bullets, starting from the direction this unit is facing

- moveToParent ()
  - moves the location of this unit to the location of its owner, based on owner ID

## Bullet script

Bullet scripts contain behaviour and spawn instructions for bullets.

The script lines are formatted the same as enemy scripts, but with a slightly different instruction and trigger set.

`[ID]:[trigger(arguments)]:[instruction1(arguments)] [instruction2(arguments)]`



### Bullet Triggers

### Bullet Instruction list