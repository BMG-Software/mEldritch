This is an example mod for mEldritch. It contains 5 rooms for the library,
four new rooms (Rooms/Library/library-samplemod-*.eldritchroom) and a
redifinition of an existing room (Rooms/Library/library-u.eldritchroom).
The new rooms must be made available to the world generator, this is done in
Config/worldgen.config. Note how the original context for the library
worldgen is used, and NumRoomDefs = ? is used to append to the list of rooms.
For the overridden room, no entry is necessary, it will just be found by the
resource loading process.

If you want to use this mod in your game, just copy the entire folder SampleMod
into your Mods folder, and add it to mods.cfg. You will have to generate a
new world in order to see the new rooms.
