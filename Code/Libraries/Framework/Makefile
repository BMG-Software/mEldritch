# Makefile in Framework

LIB=$(LIB_FRAMEWORK)

CXXFILES= src/framework3d.cpp src/frameworkutil.cpp src/uiinputmapframework.cpp \
	src/uimanagerframework.cpp

ifeq ($(PLATFORM),Windows)
CXXFILES+=src/checkforupdates.cpp
endif

INCLUDES=$(INC_CORE) $(INC_MATH)  $(INC_2D) $(INC_3D) $(INC_WORKBENCH) $(INC_UI) $(INC_AUDIO)

include $(TOP)/make_lib.mk
