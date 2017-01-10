#pragma once

// Function pointers

typedef void* (__cdecl *FishAllocFunc)(int Size);
typedef void(__cdecl *FishFreeFunc)(void* Mem);

#ifdef FGUI_DYNAMIC_ALLOC
#ifndef VMM_IMPL
#define VMM_EXTERN extern
#else
#define VMM_EXTERN
#endif

VMM_EXTERN struct {
	FishAllocFunc Allocate;
	FishFreeFunc Free;
} __fgui_vmm;
#endif

#ifdef VMM_IMPL
#ifndef FGUI_REALLOC
#define FGUI_REALLOC __fgui_Realloc
void* __fgui_Realloc(void* Mem, int OldSize, int NewSize) {
	void* New = FGUI_ALLOC(NewSize);
	memcpy(New, Mem, OldSize < NewSize ? OldSize : NewSize);
	FGUI_FREE(Mem);
	return New;
}
#endif
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

// Main functions

EXPORT FishGUI* fgui_Init(FishAllocFunc, FishFreeFunc, int Width, int Height);
EXPORT void fgui_Destroy(FishGUI* GUI);
EXPORT void fgui_SetResolution(FishGUI*, int Width, int Height);
EXPORT void fgui_DrawCanvas(FishGUI*, FishCanvas*, int X, int Y);
EXPORT void fgui_DrawScreenTo(FishGUI*, void* Output, int RedOffset, int GreenOffset, int BlueOffset, int PixelPadding, int LinePadding);

// Canvas

EXPORT FishCanvas* fgui_CanvasCreate(FishGUI*, int W, int H);
EXPORT FishCanvas* fgui_CanvasCreateFromImage(FishGUI*, byte* Data, int Len);
EXPORT void fgui_CanvasDestroy(FishGUI*, FishCanvas*);
EXPORT void fgui_CanvasDrawCanvas(FishGUI*, FishCanvas* Source, FishCanvas* Dest, int DestX, int DestY);
EXPORT void fgui_CanvasClear(FishGUI*, FishCanvas* Canvas, byte R, byte G, byte B, byte A);
EXPORT void fgui_CanvasResize(FishGUI*, FishCanvas* Canvas, int W, int H);