# untitled engine scripting reference

slightly less shit documentation for touhou suzunaan's yet to be named scripting language

## Contents

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

### Instruction list

- enemy(int type, int scriptID, float x, float y)
  - Spawns an enemy with given type and attached enemy script, at location (x, y)
  - For type info, see the *Enemy script* section.

## Enemy script

### Triggers

### Instruction list

## Bullet script

### Triggers

### Instruction list