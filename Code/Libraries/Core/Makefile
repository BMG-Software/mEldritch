# Makefile in Core

LIB=$(LIB_CORE)

CXXFILES=src/allocatorchunk.cpp src/allocator.cpp src/bufferedfilestream.cpp \
	src/checksum.cpp src/clock.cpp src/configmanager.cpp src/configparser.cpp \
	src/configvar.cpp src/customassert.cpp src/customnew.cpp src/datapipe.cpp \
	src/display.cpp src/dynamicmemorystream.cpp src/exceptiontrace.cpp \
	src/file.cpp src/filestream.cpp src/hashedstring.cpp src/inputcontext.cpp \
	src/inputsystem.cpp src/keyboard.cpp src/memorystream.cpp src/mouse.cpp \
	src/packstream.cpp src/printmanager.cpp src/profiler.cpp src/profilerhook.cpp \
	src/profilertable.cpp src/runtimeerror.cpp src/reversehash.cpp src/simplestring.cpp \
	src/stopwatch.cpp src/stringmanager.cpp src/test.cpp src/timer.cpp src/modmanager.cpp \
	src/TinyXML/tinyxml.cpp src/TinyXML/tinyxmlerror.cpp src/TinyXML/tinyxmlparser.cpp \
	src/windowwrapper.cpp src/xinputcontroller.cpp src/timedate.cpp src/printmanagerstream.cpp

ifeq ($(PLATFORM),Windows)
CXXFILES+=src/console.cpp src/consoleinput.cpp src/consolemanager.cpp src/exceptionhandler.cpp \
	src/exceptionuploadlog.cpp src/httpsocket.cpp src/mutex.cpp src/thread.cpp
endif

ifeq ($(PLATFORM),MacOSX)
OBJCFILES+=src/objcjunk.mm
endif

# Note: this should not be here...
INCLUDES=$(INC_ZLIB) $(INC_FRAMEWORK)

include $(TOP)/make_lib.mk
