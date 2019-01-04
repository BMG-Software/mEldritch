# Makefile in 3D

LIB=$(LIB_3D)

CXXFILES=src/3d.cpp src/animation.cpp src/animationstate.cpp src/animevent.cpp \
	src/animeventfactory.cpp src/bonearray.cpp src/bucket.cpp src/dynamicmeshmanager.cpp \
	src/font.cpp src/fontmanager.cpp src/irradiancevolumes.cpp src/kdtree.cpp \
	src/linebatcher.cpp src/lodepng.cpp src/material.cpp src/mesh.cpp src/meshfactory.cpp \
	src/renderercommon.cpp src/renderstates.cpp src/sdpfactory.cpp src/SDPs/sdpbase.cpp \
	src/sectorbound.cpp src/shaderdataprovider.cpp src/shadermanager.cpp \
	src/targetmanager.cpp src/texturecommon.cpp src/texturemanager.cpp \
	src/vertexdeclarationmanager.cpp src/view.cpp src/viewportpass.cpp \
	src/GL2/gl2indexbuffer.cpp src/GL2/gl2pixelshader.cpp src/GL2/gl2renderer.cpp \
	src/GL2/gl2rendertarget.cpp src/GL2/gl2shaderprogram.cpp src/GL2/gl2texture.cpp \
	src/GL2/gl2vertexbuffer.cpp src/GL2/gl2vertexdeclaration.cpp src/GL2/gl2vertexshader.cpp

ifeq ($(BACKEND), D3D)
CXXFILES += src/D3D9/d3d9constanttable.cpp src/D3D9/d3d9indexbuffer.cpp \
	src/D3D9/d3d9pixelshader.cpp src/D3D9/d3d9renderer.cpp src/D3D9/d3d9rendertarget.cpp \
	src/D3D9/d3d9shaderprogram.cpp src/D3D9/d3d9texture.cpp src/D3D9/d3d9vertexbuffer.cpp \
	src/D3D9/d3d9vertexdeclaration.cpp src/D3D9/d3d9vertexshader.cpp
endif

INCLUDES=$(INC_CORE) $(INC_MATH) $(INC_GLEW)

CXXFLAGS += -DLODEPNG_NO_COMPILE_ENCODER

include $(TOP)/make_lib.mk
