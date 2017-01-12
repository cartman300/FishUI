#pragma once

#include <math.h>
#include <string.h>

// Intrinsics
#include <emmintrin.h>

// Allows to pass custom memory allocator instead of using #define'd one
#define FGUI_DYNAMIC_ALLOC 

#ifndef FGUI_DYNAMIC_ALLOC
#define FGUI_ALLOC malloc
#define FGUI_FREE free
#define FGUI_REALLOC realloc
#else
#define FGUI_ALLOC __fgui_vmm.Allocate
#define FGUI_FREE __fgui_vmm.Free
#define FGUI_REALLOC __fgui_vmm.ReAllocate
#endif

// Types

typedef unsigned int uint;
typedef unsigned char byte;

// Helper macros

#ifndef NULL
#define NULL 0
#endif

#define EXPORT __declspec(dllexport)
#define UNREF_PARAM(x) (void)(x)