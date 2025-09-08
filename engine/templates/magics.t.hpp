#pragma once

#include <stdint.h>

#ifdef _WIN32
#ifdef UMBRA_BUILD_DLL
#define UMBRA_API __declspec(dllexport)
#else
#define UMBRA_API __declspec(dllimport)
#endif
#else
#define UMBRA_API __attribute__((visibility("default")))
#endif

#define UMBRA_VERSION_MAJOR @engine_VERSION_MAJOR@
#define UMBRA_VERSION_MINOR @engine_VERSION_MINOR@
#define UMBRA_VERSION_PATCH @engine_VERSION_PATCH@

#define UMBRA_VERSION_PACK(major, minor, patch) (((unsigned)(major) << 16) | ((unsigned)(minor) << 8) | ((unsigned)(patch)))

#define UMBRA_VERSION UMBRA_VERSION_PACK(UMBRA_VERSION_MAJOR, UMBRA_VERSION_MINOR, UMBRA_VERSION_PATCH)
#define UMBRA_VERSION_STR "@engine_VERSION@"