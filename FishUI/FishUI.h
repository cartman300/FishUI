#pragma once

typedef void* (__cdecl *FishAllocFunc)(int Size);
typedef void (__cdecl *FishFreeFunc)(void* Mem);

typedef struct {
	byte R, G, B, A;
} FishPixel;

typedef struct {
	int Width;
	int Height;
	FishPixel* Buffer;
} FishCanvas;

typedef struct {
	FishAllocFunc Allocate;
	FishFreeFunc Free;

	FishCanvas* ScreenBuffer;
} FishGUI;

EXPORT void fgui_Init(FishGUI*, FishAllocFunc, FishFreeFunc, int Width, int Height);
EXPORT void fgui_SetResolution(FishGUI*, int Width, int Height);
EXPORT void fgui_DrawCanvas(FishGUI*, FishCanvas*, int X, int Y);
EXPORT void fgui_DrawScreenTo(FishGUI*, void* Output, int RedOffset, int GreenOffset, int BlueOffset, int PixelStride, int LineStride);

// CANVAS

EXPORT FishCanvas* fgui_CanvasCreate(FishGUI*);
EXPORT void fgui_CanvasDestroy(FishGUI*, FishCanvas*);
EXPORT void fgui_CopyCanvas(FishGUI*, FishCanvas* Source, FishCanvas* Dest, int DestX, int DestY);