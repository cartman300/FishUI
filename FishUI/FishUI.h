#pragma once

// Function pointers

typedef void* (__cdecl *FishAllocFunc)(int Size);
typedef void* (__cdecl *FishReAllocFunc)(void* Mem, int Size);
typedef void(__cdecl *FishFreeFunc)(void* Mem);

#ifdef FGUI_DYNAMIC_ALLOC
#ifndef VMM_IMPL
#define VMM_EXTERN extern
#else
#define VMM_EXTERN
#endif

VMM_EXTERN struct {
	FishAllocFunc Allocate;
	FishReAllocFunc ReAllocate;
	FishFreeFunc Free;
} __fgui_vmm;
#endif

// Structs

typedef struct {
	byte R, G, B, A;
} FishPixel;

typedef struct {
	int Width;
	int Height;
	FishPixel* Buffer;
} FishCanvas;

typedef struct {
	FishCanvas* ScreenBuffer;
} FishGUI;

// Helper macros

#define __OVERBLEND(fg, bg, alpha) (((fg * alpha) + (bg * (255 - alpha))) / 255)
#define OVERBLEND(fg, bg, alpha) __OVERBLEND((fg), (bg), (alpha))

#define __CLAMP(min, a, max) (a < min ? min : (a > max ? max : a))
#define CLAMP(min, a, max) __CLAMP((min), (a), (max))

#define __LERP(a, b, amt, max) (((max - amt) * a + amt * b) / max)
#define LERP(a, b, amt, max) __LERP((a), (b), (amt), (max))

// Main functions

EXPORT FishGUI* fgui_Init(FishAllocFunc, FishReAllocFunc, FishFreeFunc, int Width, int Height);
EXPORT void fgui_Destroy(FishGUI* GUI);
EXPORT void fgui_SetResolution(FishGUI*, int Width, int Height);
EXPORT FishCanvas* fgui_GetScreenCanvas(FishGUI*);
EXPORT void fgui_DrawScreenTo(FishGUI*, void* Output, int RedOffset, int GreenOffset, int BlueOffset, int PixelPadding, int LinePadding);

// Canvas

EXPORT FishCanvas* fgui_CanvasCreate(FishGUI*, int W, int H);
EXPORT FishCanvas* fgui_CanvasCreateFromImage(FishGUI*, byte* Data, int Len);
EXPORT void fgui_CanvasDestroy(FishGUI*, FishCanvas*);
EXPORT void fgui_CanvasDrawCanvas(FishGUI*, FishCanvas* Source, FishCanvas* Dest, int DestX, int DestY);
EXPORT void fgui_CanvasClear(FishGUI*, FishCanvas* Canvas, byte R, byte G, byte B, byte A);
EXPORT void fgui_CanvasResize(FishGUI*, FishCanvas* Canvas, int W, int H);

EXPORT void fgui_CanvasDrawCanvasDepth(FishGUI*, FishCanvas* Source, FishCanvas* SourceDepth,
	FishCanvas* Dest, FishCanvas* DestDepth, int DestX, int DestY);