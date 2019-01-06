# mEldritch Source #

This is a build of the Eldritch source code released by David Pittman that
has been modified to support modding more easily. Thanks, David, for
releasing the source code to your game, and for being very helpful with
this project! You need the original game to use this, which you can get
from

<http://eldritchgame.com/>

The original source code is available from the bottom of this page:

<http://eldritchgame.com/about.html>

This build has been created by B.M. Gooding as an amalgamation of the excellent 
mEldritch made by Gunnar Zötl (gz@tset.de) and the OpenAL version of the original 
Eldritch source code to remove the FMOD programmers API dependency. 

This is all covered under the same license as the original Eldritch source code.

This version of Eldritch's source code has been modified to be built using
Makefiles with the standard development toolchain on Linux and will work
with the equivalent build tools (Mingw) on Windows 10.

Note that on both platforms, a 32 bit executable is being built.
Eldritch's source code is not 64-bit clean.

For both platforms, you can set paths to external libraries in
`Code/make_config.mk`. Those are SDL2, OpenAL, freealut and Vorbis.
Instructions for all of these steps will soon be added to the wiki.

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
Also, OpenAL is a requirement so must be linked against in the same way, 
and then a simple make in the directory Code should suffice.

Instructions will be linked soon.

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

## MacOSX ##

Unfortunately MacOSX will likely never recieve any official support from me
as I have no machine to test it with. Fortunately the original mEldritch 
does fully support OSX so head over there.

## Building on Windows ##

You need MSYS, which you can get from  <http://www.mingw.org/>
You also need to install the 32bit toolchain of mingw-w64 from
<http://www.win-builds.org/>

Make sure to install it for an MSYS hosted system. Refer to the instructions
in the wiki for info on download and installation of OpenAL, and then a simple 
make in the directory Code should suffice.

## Installing on Windows ##

Installing on Windows is just like installing on Linux, please read that
section above.

## More information on this source distribution ##
The wiki will be updated with build instructions and migrations of all of
the documentation found within this repository modified to reflect the goals
of this particular distibution in good time.
