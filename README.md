# mEldritch Source #

This is a build of the Eldritch source code released by David Pittman that
has been modified to support modding more easily. Thanks, David, for
releasing the source code to your game, and for being very helpful with
this project! You need the original game to use this, which you can get
from

<http://eldritchgame.com/>

The original source code is available from the bottom of this page:

<http://eldritchgame.com/about.html>

This build has been created by Gunnar Zötl (gz@tset.de). Everything I made
is covered by the same license as the original Eldritch source code.

This version of Eldritch's source code has been modified to be built using
Makefiles with the standard development toolchain on Linux, with XCode
command line tools on Mac OS X and with mingw-w64 on windows.  It has been
tested with the humble version of Eldritch on Windows 7, Linux (XUbuntu
14.04) and MacOSX Yosemite. It has also been tested with the windows steam
version of Eldritch on Windows 7.

Note that on all 3 platforms, a 32 bit executable is being built.
Eldritch's source code is not 64-bit clean.

For all platforms, you can set paths to external libraries in
`Code/make_config.mk`. Those are SDL2 and fmodex. You will need to
download FMODEX for your platform, details are in
`Dependencies/[PLATFORM]/FMOD/readme.txt`

After a successfull build, a call

	make install BUILD=...

will copy the compiled files for `BUILD` (debug, release, final) into the
directory `Eldritch-Source/Install/[PLATFORM]/[BUILD]`, and after that

	make binpack

will build one zip archive for each platform/build pair found in
Eldritch-Source/Install/

	make pack

will build an archive of the source code. Do this last as is cleans the
source directory and thus removes all compiled files.

## Building on Linux ##

You can link against the system wide SDL2 libraries, or build your own.
Just be sure to have a 32-bit build of libSDL2.so to link against.
Download and install the fmodex api as explained above, and then a
simple make in the directory Code should suffice.

## Installing on Linux ##

You may want to install mEldritch into a copy of the original Eldritch game
folder, as save files and stuff may not be interchangeably between Eldritch
and mEldritch.

Copy the `mEldritch` binary and the `Mods` folder from the folder
`Eldritch-Source/Install/[PLATFORM]/[BUILD]` into your Eldritch game
folder. The `Examples` Folder contains a sample mod, cunningly named
`SampleMod`. You can install that, if you like, it just adds a few rooms to
the library. In order to install a mod, just copy it into the `Mods` folder
and add it to `Mods/mods.cfg`.

The contents of the Extras folder is for modding. You don't need it to play
mods.

Start `mEldritch(.exe)` instead of `Eldritch(.exe)`. Press Control+Shift+T
or resurrect to generate a new, mEldritch compatible world.

## Building on MacOSX ##

As there are no system wide SDL2 libraries, you will probably need to
build your own. Be sure to create a 32-bit library. Download and install
the fmodex api as explained above, and then a simple make in the
directory Code should suffice.

## Installing on MacOSX ##

You may want to install mEldritch into a copy of the original `Eldritch.app`
folder, as save files and stuff may not be interchangeable between Eldritch
and mEldritch.

First, open (not run, but "show contents" or whatever that is called in
non-german) the `Eldritch.app` (or better yet, a copy of it). From the Folder
`Eldritch-Source/Install/MacOSX/[BUILD]`, copy the folders `MacOS` and
`Frameworks` into `Eldritch.app/Contents`, but be sure to merge them with
the existing folders!

If you want to open mEldritch on starting Eldritch.app, do this: open
Eldritch.app/Contents/Info.plist in a text editor and replace the string
for the key CFBundleExecutable with mEldritch (should be Eldritch before
replacing). Otherwise you can just navigate to Eldritch.app/Contents/MacOS
and start mEldritch from there.

The Mods folder must be copied from the archive to
~/Library/Application Support/Eldritch. The Examples Folder contains a
sample mod, cunningly named SampleMod. You can install that, if you like,
it just adds a few rooms to the library. In order to install a mod, just
copy it into the Mods folder and add it to Mods/mods.cfg.

Start mEldritch. Press `Control+Shift+T` or resurrect to generate a new,
mEldritch compatible world.

The contents of the Extras folder is for modding. You don't need it to play
mods.

## Building on Windows ##

You need MSYS, which you can get from  <http://www.mingw.org/>
You also need to install the 32bit toolchain of mingw-w64 from
<http://www.win-builds.org/>

Make sure to install it for an MSYS hosted system. Download and install
the fmodex api as explained above, and then a simple make in the
directory Code should suffice.

## Installing on Windows ##

Installing on Windows is just like installing on Linux, please read that
section above.

A special note if you are using the steam version of the game:
mEldritch works with the steam version of Eldritch, but you need to regenerate
the world from within mEldritch (Ctrl+Shift+T or Resurrect). mEldritch does
not support the extra steam content like the asylum, the halloween world,
and will crash if you try to enter them.

## More information about mEldritch ##

Find readme_meldritch.txt either in `Eldritch-Source/Install/[PLATFORM]/[BUILD]`
(after a call to make install) or in `Eldritch-Source/Projects/Eldritch`.
