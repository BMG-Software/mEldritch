#ifndef VERSIONS_H
#define VERSIONS_H

#ifndef BUILD_WINDOWS
#define BUILD_WINDOWS	0
#endif

#ifndef BUILD_MAC
#define BUILD_MAC		0
#endif

#ifndef BUILD_LINUX
#define BUILD_LINUX		0
#endif

#ifndef BUILD_SDL
#define BUILD_SDL		0
#endif

#ifndef BUILD_STEAM
#define BUILD_STEAM		0
#endif

#if BUILD_WINDOWS==0 && BUILD_MAC==0 && BUILD_LINUX==0
#error You must select a version to build
#endif

#define BUILD_WINDOWS_NO_SDL	BUILD_WINDOWS && !BUILD_SDL

#endif // VERSIONS_H
