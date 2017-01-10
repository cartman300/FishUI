#include "stdafx.h"

#define VMM_IMPL
#include "FishUI.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#define STBI_MALLOC FGUI_ALLOC
#define STBI_FREE FGUI_FREE
#define STBI_REALLOC_SIZED FGUI_REALLOC
#define STBI_ONLY_PNG
#include "stb_image.h"

FishGUI* fgui_Init(FishAllocFunc Alloc, FishFreeFunc Free,	int Width, int Height) {
#ifdef FGUI_DYNAMIC_ALLOC
	__fgui_vmm.Allocate = Alloc;
	__fgui_vmm.Free = Free;
#endif

	FishGUI* GUI = FGUI_ALLOC(sizeof(FishGUI));
	GUI->ScreenBuffer = NULL;

	fgui_SetResolution(GUI, Width, Height);
	return GUI;
}

void fgui_Destroy(FishGUI* GUI) {
	fgui_CanvasDestroy(GUI, GUI->ScreenBuffer);
	if (GUI != NULL)
		FGUI_FREE(GUI);
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
	fgui_CanvasDrawCanvas(GUI, Canvas, GUI->ScreenBuffer, X, Y);
}

void fgui_DrawScreenTo(FishGUI* GUI, void* Output, int ROffset, int GOffset, int BOffset, int PixelPadding, int LinePadding) {
	byte* OutputBuffer = (byte*)Output;
	FishPixel* InputBuffer = GUI->ScreenBuffer->Buffer;

	uint W = GUI->ScreenBuffer->Width;
	uint H = GUI->ScreenBuffer->Height;
	uint PixelSize = sizeof(byte) * 3 + PixelPadding;

	uint Pos = 0;
	uint ScrPos = 0;

	// TODO: Optimize
	for (uint Y = 0; Y < H; Y++) {
		for (uint X = 0; X < W; X++) {
			Pos = (Y * (W + LinePadding) + X) * PixelSize;
			ScrPos = (Y * W) + X;

			OutputBuffer[Pos + ROffset] = (InputBuffer[ScrPos].R * InputBuffer[ScrPos].A) / 255;
			OutputBuffer[Pos + GOffset] = (InputBuffer[ScrPos].G * InputBuffer[ScrPos].A) / 255;
			OutputBuffer[Pos + BOffset] = (InputBuffer[ScrPos].B * InputBuffer[ScrPos].A) / 255;
		}
	}
}

// Canvas

FishCanvas* fgui_CanvasCreate(FishGUI* GUI, int W, int H) {
	FishCanvas* Canvas = FGUI_ALLOC(sizeof(FishCanvas));
	Canvas->Width = W;
	Canvas->Height = H;
	Canvas->Buffer = FGUI_ALLOC(sizeof(FishPixel) * W * H);
	return Canvas;
}

FishCanvas* fgui_CanvasCreateFromImage(FishGUI* GUI, byte* Data, int Len) {
	int W, H, Channels;
	byte* ImgData = stbi_load_from_memory(Data, Len, &W, &H, &Channels, 0);

	FishCanvas* Canvas = fgui_CanvasCreate(GUI, W, H);
	memcpy(Canvas->Buffer, ImgData, W * H * Channels);

	stbi_image_free(ImgData);
	return Canvas;
}

void fgui_CanvasDestroy(FishGUI* GUI, FishCanvas* Canvas) {
	if (Canvas->Buffer != NULL)
		FGUI_FREE(Canvas->Buffer);
	if (Canvas != NULL)
		FGUI_FREE(Canvas);
}

// TODO: Optimize
// TODO: Alpha blending?
void fgui_CanvasDrawCanvas(FishGUI* GUI, FishCanvas* Source, FishCanvas* Dest, int DestX, int DestY) {
	UNREF_PARAM(GUI);

	uint SrcW = Source->Width;
	uint SrcH = Source->Height;
	uint DstW = Dest->Width;
	//uint DstH = Dest->Height;

	for (uint Y = 0; Y < SrcH; Y++) {
		for (uint X = 0; X < SrcW; X++) {
			uint SrcOffset = Y * SrcW + X;
			uint DstOffset = (Y + DestY) * DstW + (X + DestX);

			if (Source->Buffer[SrcOffset].A != 0)
				Dest->Buffer[DstOffset] = Source->Buffer[SrcOffset];
		}
	}
}

// TODO: Optimize
void fgui_CanvasClear(FishGUI* GUI, FishCanvas* Canvas, byte R, byte G, byte B, byte A) {
	UNREF_PARAM(GUI);

	uint SrcH = Canvas->Height;
	uint SrcW = Canvas->Width;

	for (uint Y = 0; Y < SrcH; Y++) {
		for (uint X = 0; X < SrcW; X++) {
			uint SrcOffset = Y * SrcW + X;

			Canvas->Buffer[SrcOffset].R = R;
			Canvas->Buffer[SrcOffset].G = G;
			Canvas->Buffer[SrcOffset].B = B;
			Canvas->Buffer[SrcOffset].A = A;
		}
	}
}

void fgui_CanvasResize(FishGUI* GUI, FishCanvas* Canvas, int W, int H) {
	FishCanvas* NewCanvas = fgui_CanvasCreate(GUI, W, H);

	// dest[dx, dy] = src[dx*src_width / dest_width, dy*src_height / dest_height]

	uint SrcH = Canvas->Height;
	uint SrcW = Canvas->Width;
	uint DstH = H;
	uint DstW = W;

	for (uint Y = 0; Y < SrcH; Y++) {
		for (uint X = 0; X < SrcW; X++) {
			uint DstOffset = Y * DstW + X;
			uint SrcOffset = (Y * SrcH / DstH) * SrcW + (X * SrcW / DstW);

			NewCanvas->Buffer[DstOffset].R = Canvas->Buffer[SrcOffset].R;
			NewCanvas->Buffer[DstOffset].G = Canvas->Buffer[SrcOffset].G;
			NewCanvas->Buffer[DstOffset].B = Canvas->Buffer[SrcOffset].B;
			NewCanvas->Buffer[DstOffset].A = Canvas->Buffer[SrcOffset].A;
		}
	}

	Canvas->Width = W;
	Canvas->Height = H;
	FGUI_FREE(Canvas->Buffer);
	Canvas->Buffer = NewCanvas->Buffer;
	NewCanvas->Buffer = NULL;
	fgui_CanvasDestroy(GUI, NewCanvas);
}