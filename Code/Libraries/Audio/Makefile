# Makefile in Audio

LIB=$(LIB_AUDIO)

CXXFILES=src/audio.cpp src/audiosystemcommon.cpp src/openalaudiosystem.cpp \
	 src/openalsound.cpp src/openalsoundinstance.cpp src/musicmanager.cpp \
	src/sound3dlistener.cpp src/soundcommon.cpp src/soundinstancecommon.cpp \
	src/soundmanager.cpp

# src/MIDI/midicontrollerevent.cpp

# these compile on linux but seem unused.
#CXXFILES+=src/MIDI/midi.cpp src/MIDI/midievent.cpp src/MIDI/midimetaevent.cpp \
#	src/MIDI/midimidievent.cpp src/MIDI/midinoteoffevent.cpp src/MIDI/midinoteonevent.cpp \
#	src/MIDI/midipitchbendevent.cpp src/MIDI/midiprogramchangeevent.cpp

INCLUDES=$(INC_CORE) $(INC_MATH) #include OpenAL

include $(TOP)/make_lib.mk
