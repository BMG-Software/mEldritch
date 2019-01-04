# mEldritch #

This is a build of the Eldritch source code released by David Pittman that
has been modified to support modding more easily. Thanks, David, for
releasing the source code to your game, and for being very helpful with
this project! You need the original game to use this, which you can get
from

<http://eldritchgame.com/>

The original source code is available from the bottom of this page:

<http://eldritchgame.com/about.html>

This build has been created by Gunnar Zötl (gz@tset.de).  Everything I made
is covered by the same license as the original Eldritch source code.

This version of Eldritch has been tested with the humble version of Eldritch
on Windows 7, Linux (XUbuntu 14.04) and MacOSX Yosemite. It has also been
tested with the windows steam version of Eldritch on Windows 7.

Major changes to the original Eldritch source are:

- unified build system (Makefiles & gcc) on all 3 platforms
- ported editor and all tools (except for the ContentSyncer) to Mac and
  Linux
- revised loader for game assets, for easy overriding of game assets in mods
- revised config file loader, with a similar purpose
- changed a few editor key bindings from the original, since they clashed
  with stuff on Linux or Mac. Also, made a few other keys available in the
  game, like Quicksave/load (using the latter will leave a mark on your game
  screen, though ;) )
- screenshots working on all platforms
- made all spawners that make sense available in the editor
- made a subtool to choose the theme for your room from within the editor
- an entry point to mods having new worlds within the game
- added PNG loading for textures
- and a load of smaller things to facilitate the above
- provided example mods, and something resembling the beginnings of a
  documentation

Still on my todo list for coming builds are:

- have your Mods directory in user specific places, like the user home or
  the user config dir, as well as in the main game dir (currently slightly
  different on Mac).
- dependencies among mods
- implement a simpler way to view a room in-game
- change the font renderer for the font generator to allow for non-
  antialiased fonts
- better error handling that crashing... at least give an indication of why
- a cursor for the input widget :)

and some other stuff.

There is no "real" documentation yet, assorted notes follow.

## Installation ##

You may want to install mEldritch into a copy of the original Eldritch game
folder, as save files and stuff may not be interchangeable between Eldritch
and mEldritch. This is especially true for the steam version of Eldritch.

### Linux / Windows ###

Copy the `mEldritch` binary and the `Mods` folder from the archive into your
Eldritch game folder. The `Examples` Folder contains a few sample mods. In
order to install a mod, just copy it into the `Mods` folder and add it to
`Mods/mods.cfg`.

Start `mEldritch(.exe)` instead of `Eldritch(.exe)`. Press `Control+Shift+T`
or resurrect to generate a new, mEldritch compatible world.

#### A Note for the steam version ####

mEldritch works with the steam version of Eldritch, but you need to regenerate
the world from within mEldritch (`Ctrl+Shift+T` or Resurrect). mEldritch does
not support the extra steam content like the asylum, the halloween world,
and will crash if you try to enter them.

### MacOS X ###

First, open (not run, but "show contents" or whatever that is called in
non-german) the `Eldritch.app` (or better yet, a copy of it). From the Archive,
copy the folders `MacOS` and `Frameworks` into `Eldritch.app/Contents`, but
be sure to merge them with the existing folders!

If you want to open mEldritch on starting Eldritch.app, do this: open
`Eldritch.app/Contents/Info.plist` in a text editor and replace the string
for the key `CFBundleExecutable` with `mEldritch` (should be `Eldritch` before
replacing). Otherwise you can just navigate to `Eldritch.app/Contents/MacOS`
and start mEldritch from there.

The `Mods` folder must be copied from the archive to
`~/Library/Application Support/Eldritch`. The `Examples` Folder contains a
few sample mods. In order to install a mod, just copy it into the `Mods`
folder and add it to `Mods/mods.cfg`.

### The Extras Folder ###

The tool executables (see below) are in the Extras folder. They don't need
to be in a special place. The tools.cfg is for the mMeshCompiler, you will
probably want to put it into your project. Also in the Extras folder is:

- an `Examples` Folder, which currently only has a template mod and a few
  examples.
- the `Raw/Rooms` folder which has the original Eldritch room files with
  some modified for mEldritch
- the `Raw/Config` folder, which has the original Eldritch Config files with
  some modified for mEldritch
- the `Raw/Fonts` folder, which has the freesans10 truetype font and
  description needed to make the replacement font for the level editor
- The `Tools` folder, which is the original Tools folder as distributed with
  Eldritch's source code

## Modding ##

a mod is stored in a directory beneath `Mods`, with a file named `mod.config`
inside it. See the template mod in the `Examples` folder. Active mods are
added to `Mods/mods.cfg`. A mod is only loaded if it is added there.

Config files are loaded like this:

- first load the compiled config file from the cpk file in `Eldritch/`
- then load the uncompiled file from there
- then load the compiled config file from the Config dir in `Eldritch/`
- then load the uncompiled file from there
- then for the first mod `Mods/mods.cfg` load mod.config, and for all
  config files listed therein do the above steps in the mods' dir
- for the second and all following mods listes in `Mods/mods.cfg` do the
  same

all config files found with this method are loaded, not just the first one
found. This allows for easy modifications of config files without having to
copy the whole thing. But note: in mods only the config files listed in
mod.config are loaded!

Resources are loaded like this:

- first try the filesystem in the last mod listed in `Mods/mods.cfg`,
- then try the cpk files fromn the last mod listed in `Mods/mods.cfg`
- then the same for the second-but-last file, and so on backwards until
  the first mod.
- then try the filesystem in `Eldritch/`
- finally try the cpk files in `Eldritch/`

The first resource file found for a given is loaded. This allows to easily
override resources without having to create an entire config structure
just to change a font or whatever. But it also means that you will have to
choose unique names for your module specific resource files, because later
mods might override them.

Beware: the Eldritch engine is very picky with errors in config files. Things
like files that are not available on load or other stuff are more often than
not the cause of a crash!

## Rooms ##

A room is stored in a directory and with a filename following this pattern:

	Rooms/[world]/[theme]-([stuff]-)[exits](-[version]).eldritchroom

Room themes and exits are determined from the room's filename. `[theme]` is
the rooms' theme, `[exits]` holds the exits, which are any combination of n,
s, w, e, d and u, and an optional version number for multiple rooms with the
same combination of exits. `[stuff]` may be anything, it is not parsed. It
could for example be your mod name and/or a special room name. The mod name
would help to make the level names unique.

A theme is not the same as the world the room was created in. It is a theme
within that world, like, all rooms in Dagon world that have ruins in them.
The world generator uses this information to generate more rooms of one
theme adjacent to another one.

## Level editor ##

The transparent box in the middle of the screen is the brush. It can be used
to fill areas. Use the keys listed below to manipulate the brush. The green
line in the brush points north.

Currently the only way to view an edited room ingame is to add it to the
room list for the world your room should be in, and then regenerate the
world, either by pressing Ctrl-Shift-T or resurrect. I plan to do something
about this in a future release, but that is how it is now.

### Keys ###

Prefix S- means left shift, C- left control and A- left alt keym, this is
on a US keyboard. ML, MM and MR are the left, middle and right mouse buttons
respectively, MW is the mouse wheel.

	F1:		next sub tool
	S-F1:	prev sub tool
	F2:		toggle sub tool Voxelset
	F3:		toggle sub tool Palette
	F4:		toggle sub tool Spawners

	W/A/S/D	move camera
	Q/E		move camera up/down
	S-move	fast move

	A-1		new 1x1x1
	A-2		new 1x2x1
	A-3		new 1x1x2
	A-4		new 2x2x1

	C-S		quicksave
	F5		save
	F6		load
	F7		clear
	A-F7	clear with floor
	F8		fill
	A-F8	clear with shell

	ML		place
	C-ML	replace

	MR		delete
	S-MR	pick voxel from room for drawing
	C-MR	add voxel from room to drawing palette

	MM		move brush
	S-MM	expand brush
	C-MM	move brush to contain voxel

	R		place spawner
	S-R		remove spawner
	C-R		rotate spawner

	C-A		maximize brush
	C-D		reset brush
	F/Enter	fill brush
	S-F/Enter	erase brush

	K		move brush west
	S-K		expand brush west
	C-K		shrink brush west

	L		move brush east
	S-L		expand brush east
	C-L		shrink brush east

	I		move brush north
	S-I		expand brush north
	C-I		shrink brush north

	O		move brush south
	S-O		expand brush south
	C-O		shrink brush south

	,		move brush down
	S-,		expand brush down
	C-,		shrink brush down

	.		move brush up
	S-.		expand brush up
	C-.		shrink brush up

	C-S-S	screenshot

In the Palette tool:

	ML		select voxel for drawing
	S-ML	add voxel to drawing palette

In the Spawner tool:

	ML		select spawner

## Ingame keys ##

many of these also work in the normal game

	W/A/S/D	move
	Q/E		lean left/right
	C		toggle crouch
	Space	jump / climb
	lShift	run

	F		use / pick up
	Z		drop
	X		switch weapons
	MW		switch weapons
	ML		attack
	MR		use power

	Escape	pause
	A-F4	exit
	F5		quicksave
	F6		quickload (using this will mark you as a cheater, though that currently has no effect)

	A-Enter	toggle fullscreen
	C-T		proxy return to hub (new worlds, same hub), like Return to Library.
	C-S-T	full restart (new hub), like Resurrect.
	Tab		toggle editor
	C-S-S	screenshot
	C-H		hide HUD
	C-S-H	hide HUD and hands. Note that if you save while this is in effect, the hands
			will be invisible after loading. If that happens, press S-H to show them.
	S-H		show HUD and hands

In menus or dialogs:

	Enter	accept
	Escape	cancel
	ML		select
	Left, Right, Up, Down	what the name suggests

## Config files ##

Syntax (from configparser.cpp):

- `#` begins a comment
- `#!` and `!#` indicate the start and end of block comments
- `@` declares a new macro
- `@@` inserts the current macro in the name
- `&` declares a counter (for RHS value) or preincrements and inserts the counter (for LHS name)
- `^` inserts the counter (for LHS name) without incrementing
- `?` retrieves the previous value for a counter (for RHS value) (new in mEldritch)

A Macro basically serves as an array. This:

	numbla=&
	@bla
	@@&=...
	@@&=...

creates and fills an array. `numbla` will contain number if items later.
`@bla` declares a macro, `@@` later retrieves the value of the current macro.
`numbla=&` declares a counter, initializing it to 0. `@@&` retrieves the value
of the curremt macro, and preincrements and retrieves the current counter,
so the above example declares variables `bla0`, `bla1`.

Counters are not bound to the arrays

New in mEldritch:

	numbla=?

retrieves the current value for `numbla` from the environment (a.k.a previous
config files or a previous use in the same config file) and makes it the
current value for subsequent references to `&` and `^`. Also writes back the
final value of this counter to numbla. This is intended to append things
to "arrays". Beware: Only works in uncompiled config files!

Config vars may be grouped into contexts, a context is created by a word in
brackets:

	[ThisCreatesAContext]

All vars after that are created in that context, until the next context
definition appears. You can go back to the root context by using `[]`.
Contexts don't nest, there is only one level of contexts. The root context
is also a context (with the name ""), so things defined there don't clash
with context names.

config vars are parsed into a hash structure, vars with same name in the
same context override each other. This is also true if they are defined in
separate config files.

Beware: You should always end your config files in a newline, otherwise the
last value (if it consists of just a single char) might not be read.

In a mod, you should not replace an entire config file with your own stuff,
unless there is good reason to do so. Otherwise you might break things for
other mods loaded after yours. Also, you should base your mod config files
on those that come with mEldritch. The reason for this is that the config
files for the steam version of the game may refer to stuff that is not
available in mEldritch, and the engine's usual answer to that sort of stuff
is to crash.

### Some important config files ###

`default.config`
	:startup configuration.

`worldgen.config`
:	lists of rooms for the world generator. Rooms for a specific world have
	to be appended to the rooms list for a specific context. Check here for
	context names.

`spawners.config`
:	defines the spawners for objects and AIs used in the game.

`tools.config`
:	settings for the editor, amongst other things makes those spawners
	defined in spawners.config available to the editor.

There's a lot more, explore and document.

## Spawners ##

`spawner_ladder5` and `spawner_ladder3` attaches in the middle, `spawner_ladder4`
one below the middle

## Tools ##

The tools are mostly just the original versions, some with minor changes to
get them running cross-platform. The only tool that has received more work
is the FontCompiler, which was completely dependent on windows font rendering,
and David's monitor size...

the m prefix is just there to not clash with David's original versions of
the FontGenerator and ConfigCompiler, which are present in the source
archive. And also because the moddable Eldritch build is called mEldritch.

### mChecksum ###

	mChecksum <file> [-s]

Computes a checksum for <file>. append -s for short output.

### mConfigCompiler ###

	mConfigCompiler <infile.config> <outfile.ccf>

Compiles config file <infile.config> to a binary format and writes that to
<outfile.ccf>.

### mFilePacker ###

	mFilePacker <infile> <packfile.cpk> [-c]

appends the <infile> to the archive <packfile.cpk>. The complete path as
specified for <infile> will be used in the packfile. The file will be
appended uncompressed unless -c is specified.

	mFilePacker -u <packfile.cpk>

unpacks the entire contents of <packfile.cpk>

	mFilePacker -l <packfile>

lists the contents of <packfile.cpk>

### mFontGenerator ###

	mFontGenerator <infile.font> <outfontfile.fnp> <outimagefile.tga> [<runtimeimagepath>]

generates a fnp file and an image file from a font description in
<infile.font> and a TrueType font file (which is specified in <infile.font>).
You can optionally specify the runtime image path for the font, which is
relative to the Mod or Eldritch dir, normally "Textures/Fonts". If you don't
specify it, the path ot <outimagefile.tga> will be used for the runtime path.
Note that the <outimagefile.tga> is basically a pattern which will be
modified with the locale name and stuff.

Currently the FontGenerator can only generate anti-aliased fonts.

### mMeshCompiler ###

	MeshCompiler <inmesh.xml> <outmesh.cms> -l

compiles a mesh in xml format to a mesh in cms format that the Eldritch
engine can use. -l specifies long indices, else shorts are used.
The mesh in xml format is created by an exporter for blender that can be
found in the Extras/Tools directory. David used blender 2.49b, so the
exporter was written for that.

## Troubleshooting ##

That's a tough one. The engine does not like it when stuff that is referenced
in the config files or the save files is not actually there. So if mEldritch
crashes, it is usually something to do with that.

- If you just installed mEldritch, and it crashes upon startup, try deleting
  your main.eldritchmastersave file. This happens mostly with steam Eldritch,
  as that one has stuff mEldritch does not support.
- If you removed a mod, and mEldritch crashes, try deleting your
  main.eldritchmastersave file. Most likely the mod defined some resources
  that are not presend any more, but are still refwerenced in the save file.
- If you are making a mod, and mEldritch crashes when it is loaded, chances
  are, you have a typo somewhere. For example, your spawner references an
  entity that is not defined, or an external asset like a texture is not
  found. I am slowly adding code to indicate what went wrong, check
  meldritch_log.txt for hints.
