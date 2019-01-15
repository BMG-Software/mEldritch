# Top Level Makefile for Eldritch Engine
#
# targets:
#	debug		builds with debugging info (-g) and dev tools, but
#				without optimization
#	release		builds with dev tools and optimization but without
#				debugging info
#	final		like release but without dev tools
#	install		collect all generated programs into $(INSTDIR)/$(PLATFORM)/$(BUILD)
#	clean		remove everything that was built
#	pack		creates a tar archive of the current source tree, without
#				the fmod stuff!
#	binpack		creates tar archives of the currently available binaries
# vars:
#	PLATFORM	linux, windows or mac, empty to attempt auto detection
#	BUILD		which build, debug, release or final. Used for install target

# compiler and flags
CC			=gcc
CXX			=g++
CXXFLAGS	=-std=c++11 -Wall -Isrc $$(INCLUDES)
LDFLAGS		=

# command for automatic dependency generation
DEPDIR		=_AutoDependencies
MAKEDEPEND	=gcc -M -I. $$(CXXFLAGS) -o $$@ $$<

# command to build libraries
AR			=ar

# Build version: debug release final, default is debug
# final has no level editor!
BUILD		?=debug

# top dir, relative from source dirs
TOP			=../..

# list of builds we provide
BUILDS		=debug release final

# Builds
# -fno-strict-aliasing on higher optimisation levels is necessary because
# of some dereferencing of typecast pointers in the code
ifeq ($(BUILD),debug)
CXXFLAGS	+=-g -D_DEBUG=1
LDFLAGS		+=-g
else ifeq ($(BUILD),release)
CXXFLAGS	+=-O3 -fno-strict-aliasing
else
CXXFLAGS	+=-O3 -fno-strict-aliasing -D_FINAL=1
endif

VERSION=$(shell cat Build)

# list of platforms we build for
PLATFORMS=Linux MacOSX Windows

# Auto detect platforms
ifndef PLATFORM
PLATFORM=$(shell uname)
ifeq ($(PLATFORM),Darwin)
PLATFORM			=MacOSX
else ifeq ($(PLATFORM),Linux)
PLATFORM			=Linux
else
PLATFORM			=Windows
endif
endif

# BUILDFLAGS specific settings
ifeq ($(PLATFORM),Linux)
BUILDFLAGS	=-DBUILD_LINUX=1 -DBUILD_SDL=1
BACKEND		=GL
SYSCXXFLAGS	=-m32 -no-pie
SYSLDFLAGS	=-Wl,-R./ -m32 -no-pie
SYSLIBS		=-lGL -lpthread -ldl -lalut -lopenal -lvorbisfile
else ifeq ($(PLATFORM),MacOSX)
CC			=MACOS_DEPLOYMENT_TARGET=10.6 gcc
CXX			=MACOS_DEPLOYMENT_TARGET=10.6 g++
BUILDFLAGS	=-DBUILD_MAC=1 -DBUILD_SDL=1
BACKEND		=GL
SYSCXXFLAGS	=-m32
SYSLDFLAGS	=-Wl,-rpath,@executable_path/../Frameworks -m32
SYSLIBS		=-Wl,-framework,OpenGL -Wl,-framework,Cocoa
else ifeq ($(PLATFORM),Windows)
BUILDFLAGS	=-DBUILD_WINDOWS=1
BACKEND		=D3D
SYSCXXFLAGS	=-m32
SYSLDFLAGS	=-Wl,-R./ -m32 -mwindows -Wl,--subsystem,windows
SYSLIBS		=-ld3d9 -ld3dx9_38 -lxinput -ldxguid -ldinput8 -lwsock32 -lws2_32 -lopengl32 -static -lgcc -lstdc++ -lwinpthread
EXE			= .exe
else
$(error PLATFORM undefined, can bei either Linux, MacOSX or Windows, or none to attempt autodetection.)
endif

BUILDFLAGS	+=-DGLEW_STATIC

include make_config.mk

# paths relevant for building and installing
INST_PATH	=$$(TOP)/$$(INSTDIR)
BUILD_PATH	=$$(BUILD)/$$(PLATFORM)
DEP_PATH	=$$(DEPDIR)/$$(PLATFORM)

# subdirs and dependencies
INC_2D		=-I$(TOP)/Libraries/2D/src
LIB_2D		=$(TOP)/Libraries/2D/$$(BUILD_PATH)/lib2D.a
INC_3D		=-I$(TOP)/Libraries/3D/src
LIB_3D		=$(TOP)/Libraries/3D/$$(BUILD_PATH)/lib3D.a
INC_AUDIO	=-I$(TOP)/Libraries/Audio/src
LIB_AUDIO	=$(TOP)/Libraries/Audio/$$(BUILD_PATH)/libAudio.a
INC_CORE	=-I$(TOP)/Libraries/Core/src
LIB_CORE	=$(TOP)/Libraries/Core/$$(BUILD_PATH)/libCore.a
INC_FRAMEWORK=-I$(TOP)/Libraries/Framework/src
LIB_FRAMEWORK=$(TOP)/Libraries/Framework/$$(BUILD_PATH)/libFramework.a
INC_MATH	=-I$(TOP)/Libraries/Math/src
LIB_MATH	=$(TOP)/Libraries/Math/$$(BUILD_PATH)/libMath.a
INC_RODIN	=-I$(TOP)/Libraries/Rodin/src
LIB_RODIN	=$(TOP)/Libraries/Rodin/$$(BUILD_PATH)/libRodin.a
INC_UI		=-I$(TOP)/Libraries/UI/src
LIB_UI		=$(TOP)/Libraries/UI/$$(BUILD_PATH)/libUI.a
INC_WORKBENCH=-I$(TOP)/Libraries/Workbench/src
LIB_WORKBENCH=$(TOP)/Libraries/Workbench/$$(BUILD_PATH)/libWorkbench.a
INC_ZLIB	=-I$(TOP)/Libraries/zlib/src
LIB_ZLIB	=$(TOP)/Libraries/zlib/$$(BUILD_PATH)/libzlib.a
INC_GLEW	=-I$(TOP)/Libraries/GLEW/src
LIB_GLEW	=$(TOP)/Libraries/GLEW/$$(BUILD_PATH)/libGLEW.a

# No user servicable parts below

CXXFLAGS += $(BUILDFLAGS)

export

########################################################################

LIBDIRS=Libraries/Core Libraries/2D Libraries/3D Libraries/Math Libraries/zlib Libraries/GLEW \
	Libraries/Audio Libraries/Framework Libraries/Rodin Libraries/UI Libraries/Workbench

PRGDIRS=Projects/Eldritch Tools/Checksum Tools/ConfigCompiler Tools/FilePacker \
	Tools/MeshCompiler Tools/FontGenerator

DOCFILES=../CHANGELOG.txt ../readme_meldritch_source.txt ../Projects/Eldritch/readme_meldritch.txt

# TODO: Tools/ContentSyncer

DIRS=$(LIBDIRS) $(PRGDIRS)

.PHONY: debug release final all install clean pack binpack

debug:
	make all BUILD=debug

release:
	make all BUILD=release

final:
	make all BUILD=final

.PHONY: $(DIRS)

all: $(DIRS)

$(DIRS):
	make -C $@ BUILD=$(BUILD)

# TODO add config
install: all
	@mkdir -p $(INSTDIR)
	@for d in $(PRGDIRS); do \
		make -C $$d install BUILD=$(BUILD); \
	done
	@if [ "$(PLATFORM)" = "MacOSX" ]; then \
		mkdir -p $(INSTDIR)/$(PLATFORM)/$(BUILD)/Frameworks; \
		cp /usr/local/lib/libSDL2*.dylib $(INSTDIR)/$(PLATFORM)/$(BUILD)/Frameworks; \
		install_name_tool -change ./libfmodex.dylib @executable_path/../Frameworks/libfmodex.dylib $(INSTDIR)/$(PLATFORM)/$(BUILD)/$(ELDRITCH); \
		install_name_tool -change /usr/local/lib/libSDL2-2.0.0.dylib @executable_path/../Frameworks/libSDL2.dylib $(INSTDIR)/$(PLATFORM)/$(BUILD)/$(ELDRITCH); \
		mkdir -p $(INSTDIR)/$(PLATFORM)/$(BUILD)/MacOS; \
		mv $(INSTDIR)/$(PLATFORM)/$(BUILD)/mEldritch $(INSTDIR)/$(PLATFORM)/$(BUILD)/MacOS; \
	fi
	make -C ../Projects/Eldritch install

clean:
	@for d in $(DIRS); do \
		make -C $$d clean; \
	done
	make -C ../Projects/Eldritch clean
	rm -rf $(INSTDIR)
	@for f in $(DOCFILES); do \
		n=../html/`basename $$f .txt`.html; \
		rm -f $$n; \
	done

pack: clean
	@(cd ../..; \
	ARCHIVE=mEldritch-Source-$(VERSION).zip; \
	rm -f $$ARCHIVE; \
	zip -r $$ARCHIVE Eldritch-Source/*.txt; \
	zip -ur $$ARCHIVE Eldritch-Source/Projects; \
	zip -ur $$ARCHIVE Eldritch-Source/Code; \
	for f in `find Eldritch-Source/Dependencies -name readme.txt`; do \
		zip -ur $$ARCHIVE $$f; \
	done \
	)

binpack:
	@(cd $(INSTDIR); \
	ARCHIVEDIR=$(shell pwd)/../..; \
	for B in $(BUILDS); do \
		for P in $(PLATFORMS); do \
			if [ -d $$P/$$B ]; then \
				ARCHIVE=$$ARCHIVEDIR/mEldritch-$$P-$$B-$(VERSION).zip; \
				rm -f $$ARCHIVE; \
				( cd $$P/$$B; zip -r $$ARCHIVE *); \
			fi; \
		done; \
	done \
	)

# local stuff

commit:
	thg
	hg identify --num > Build

html:
	for f in $(DOCFILES); do \
		n=`basename $$f .txt`; \
		o=../html/$$n.html; \
		sed -e"s/%NAME%/$$n/" < ../html/header > $$o; \
		lunamark -X notes,definition_lists,pandoc_title_blocks $$f >> $$o; \
		sed -e"s/%NAME%/$$n/" < ../html/footer >> $$o; \
	done
