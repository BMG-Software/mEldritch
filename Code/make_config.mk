# name of executable
ELDRITCH	=mEldritch

# additional C++ flags

# alternative hashing implementation with less collisions.
# Only use if you're doing a full conversion! You need to recompile all
# config and font files!
#CXXFLAGS 	+=-DUSE_HASH_FNV1A=1

# where to put executables
INSTDIR		=../Install
INST_EXTRA_DIR=/Extras

# where is lib_sdl2 (includes are (currently) in the source tree
ifeq ($(PLATFORM),Windows)
LIB_SDL2	=$(TOP)/Libraries/Core/lib/SDL2.lib
else
LIB_SDL2	=-L/usr/local/lib -lSDL2
endif

INC_OPENAL 	=-I/usr/include/AL
LIB_OPENAL	=-L/usr/lib/i386-linux-gnu
