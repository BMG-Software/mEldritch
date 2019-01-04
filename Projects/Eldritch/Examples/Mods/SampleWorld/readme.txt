This is an example world style mod for mEldritch. It contains a world with
3 levels and custom textures. It also contains a tools.cfg file to add the
world voxels to the level editor for the purpose of building rooms for this
mod.

If you want to use this mod in your game (it's really boring, though... the
most exciting thing about it is that it exists), just copy the entire folder
SampleWorld into your Mods folder, and add it to mods.cfg. It will then show
up in the new book.

What follows is a short description of how I built this:

Create the mod.config. For this just use the one from the Template mod and
fill in your own stuff. Note that as we are creating a world style mod, we
will have a value for InitialLevel.

Create a basic Config/world.config. At this point, there does not need to be
much, the initial world.config looks basically like this:

------------------------------------SNIP------------------------------------

[EldritchWorld]

NumWorldDefs = ?
@ WorldDef
@@& = "WorldDef_SampleWorld"

# ******************************* World def *******************************

[WorldDef_SampleWorld]
Tileset			= "Textures/World/tiles-sampleworld_NODXT.tga"
ColorGrading	= "Textures/Post/color-grading-default_NODXT.tga"

NumVoxelDefs = &
@ VoxelDef
@@&	= "Voxel_Builder"
@@& = "Voxel_SampleWorld_Tile_1"
@@& = "Voxel_SampleWorld_Tile_2"
#...
@@& = "Voxel_SampleWorld_Unbreakable"

# ******************************* Voxel defs ******************************

# no need to define this as it's the same for all worlds
# [Voxel_Builder]
# VoxelValue = 127
# SideTile	= 63
# TopTile	= 63
# BottomTile	= 63

[Voxel_SampleWorld_Tile_1]
VoxelValue	= 1
SideTile	= 0
TopTile		= 0
BottomTile	= 0

[Voxel_SampleWorld_Tile_2]
VoxelValue	= 2
SideTile	= 1
TopTile		= 1
BottomTile	= 1

#...

[Voxel_SampleWorld_Unbreakable]
VoxelValue	= 129
SideTile	= 62
TopTile		= 62
BottomTile	= 62

------------------------------------SNIP------------------------------------

Create the voxel texture map. The textures are numbered from 0 in the top
left to 63 in the bottom right, going from left to right, top to bottom.
Texture number 63 is always the Voxel_Builder texture. There is no fixed
value for the unbreakable voxel texture, I just chose 62.

Now create a basic worldgen.config. Here the rooms are listed, and how to
create a maze from your rooms. You will have a few feature rooms, as the
rooms containing the entry and exit points to levels are considered feature
rooms. Add those first, as they will allow you to explore your creation. You
can set the Library_Hub level as the PrevLevel for your first and the
NextLevel for your last level, until you have something more fitting.

Now start adding rooms. Keep in mind that rooms can be transformed, so
adding just one room with exits north and east will cover all combinations
for such corner rooms (n-e, n-w, s-e, s-w). You can nicely observe this in
the SampleWorld, as when creating the rooms, the green line on the ground
always pointed north, and the blue line always pointed east. The rooms in
the SampleWorld mod is the minimal amount of rooms that cover every
combination of exits.

Now refine the world.config by adding ambient lights, fog, more voxels, and
whatever, look at the world.config file in Extras/Raw/Config for inspiration.
Refine worldgen.config by adding a proper exit from your world.

Add your start level as InitialLevel to your mod.config. You may want to
take a screenshot and use that as an image for your mod. You will have to
scale it down to 256x144 pixels for that. And don't forget to add the strings
your mod references (like the values for Name in the level definitions).

Btw there is no need to name the config files as for the original game, I
just do so in order to make it easier to compare this mod with the original
game files.
