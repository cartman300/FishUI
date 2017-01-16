#pragma once

#include <string.h>

#ifdef FGUI_WINDOWS_DLL
#include <intrin.h>
#define FGUI_DYNAMIC_ALLOC 
#define EXPORT __declspec(dllexport)
#define memset32 __stosd
#else
#ifndef EXPORT
#define EXPORT
#endif
#endif

// Allows to pass custom memory allocator instead of using #define'd one
#ifndef FGUI_DYNAMIC_ALLOC
#include <stdlib.h>
#define FGUI_ALLOC malloc
#define FGUI_FREE free
#define FGUI_REALLOC realloc
#else
typedef void* (__cdecl *FishAllocFunc)(int Size);
typedef void* (__cdecl *FishReAllocFunc)(void* Mem, int Size);
typedef void(__cdecl *FishFreeFunc)(void* Mem);

#ifndef FISH_VMM_IMPL
#define FISH_VMM_EXTERN extern
#else
#define FISH_VMM_EXTERN
#endif

FISH_VMM_EXTERN struct {
	FishAllocFunc Allocate;
	FishReAllocFunc ReAllocate;
	FishFreeFunc Free;
} __fish_vmm;

#define FGUI_ALLOC __fish_vmm.Allocate
#define FGUI_FREE __fish_vmm.Free
#define FGUI_REALLOC __fish_vmm.ReAllocate

#ifdef FISH_VMM_IMPL
EXPORT void fish_Init(FishAllocFunc Alloc, FishReAllocFunc ReAlloc, FishFreeFunc Free) {
	FGUI_ALLOC = Alloc;
	FGUI_REALLOC = ReAlloc;
	FGUI_FREE = Free;
}
#endif
#endif

// Types
typedef unsigned int uint;
typedef unsigned char byte;

#if !defined(true) && !defined(false)
typedef byte bool;
#define true 1
#define false 0
#endif