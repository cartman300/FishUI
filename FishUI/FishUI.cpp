#include "stdafx.h"
#include "FishUI.h"

void fgui_Init(FishGUI* GUI, FishAllocFunc Alloc, FishFreeFunc Free, int Width, int Height) {
	for (uint i = 0; i < sizeof(FishGUI); i++)
		((uint*)GUI)[i] = 0;

	GUI->Allocate = Alloc;
	GUI->Free = Free;

	fgui_SetResolution(GUI, Width, Height);
}

void fgui_SetResolution(FishGUI* GUI, int W, int H) {
	if (W == 0 && H == 0) {
		if (GUI->ScreenBuffer != NULL) {
			fgui_CanvasDestroy(GUI, GUI->ScreenBuffer);
			GUI->ScreenBuffer = NULL;
		}

		return;
	}

	if (GUI->ScreenBuffer != NULL)
		fgui_CanvasDestroy(GUI, GUI->ScreenBuffer);
	GUI->ScreenBuffer = fgui_CanvasCreate(GUI, W, H);
}

void fgui_DrawCanvas(FishGUI* GUI, FishCanvas* Canvas, int X, int Y) {
	fgui_CopyCanvas(GUI, Canvas, GUI->ScreenBuffer, X, Y);
}

void fgui_DrawScreenTo(FishGUI* GUI, void* Output, int ROffset, int GOffset, int BOffset, int PixelStride, int LineStride) {
	byte* OutputBuffer = (byte*)Output;
	FishPixel* InputBuffer = GUI->ScreenBuffer->Buffer;

	uint W = GUI->ScreenBuffer->Width;
	uint H = GUI->ScreenBuffer->Height;
	uint PixelSize = sizeof(byte) * 3 + PixelStride;
	
	uint Pos = 0;
	uint ScrPos = 0;

	for (uint Y = 0; Y < H; Y++) {
		for (uint X = 0; X < W; X++) {
			Pos = (Y * W + LineStride) + X * (PixelSize + PixelStride);
			ScrPos = (Y * W) + X;

			OutputBuffer[Pos + ROffset] = (InputBuffer[ScrPos].R * InputBuffer[ScrPos].A) / 255;
			OutputBuffer[Pos + GOffset] = (InputBuffer[ScrPos].G * InputBuffer[ScrPos].A) / 255;
			OutputBuffer[Pos + BOffset] = (InputBuffer[ScrPos].B * InputBuffer[ScrPos].A) / 255;
		}
	}

}

// CANVAS

FishCanvas* fgui_CanvasCreate(FishGUI* GUI, int W, int H) {
	FishCanvas* Canvas = GUI->Allocate(sizeof(FishCanvas));
	Canvas->Width = W;
	Canvas->Height = H;
	Canvas->Buffer = GUI->Allocate(sizeof(FishPixel) * W * H);
}

void fgui_CanvasDestroy(FishGUI* GUI, FishCanvas* Canvas) {
	GUI->Free(Canvas->Buffer);
	GUI->Free(Canvas);
}

void fgui_CopyCanvas(FishGUI* GUI, FishCanvas* Source, FishCanvas* Dest, int DestX, int DestY) {

}