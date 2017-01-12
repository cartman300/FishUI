#include "stdafx.h"

#define VMM_IMPL
#include "FishUI.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#define STBI_MALLOC FGUI_ALLOC
#define STBI_FREE FGUI_FREE
#define STBI_REALLOC FGUI_REALLOC
#define STBI_ONLY_PNG
#include "stb_image.h"

FishGUI* fgui_Init(FishAllocFunc Alloc, FishReAllocFunc ReAlloc, FishFreeFunc Free, int Width, int Height) {
#ifdef FGUI_DYNAMIC_ALLOC
	__fgui_vmm.Allocate = Alloc;
	__fgui_vmm.ReAllocate = ReAlloc;
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

FishCanvas* fgui_GetScreenCanvas(FishGUI* GUI) {
	return GUI->ScreenBuffer;
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
void fgui_CanvasDrawCanvas(FishGUI* GUI, FishCanvas* Source, FishCanvas* Dest, int DestX, int DestY) {
	UNREF_PARAM(GUI);

	uint SrcW = Source->Width;
	uint SrcH = Source->Height;
	uint DstW = Dest->Width;
	uint DstH = Dest->Height;

	if (SrcW == DstW && SrcH == DstH && DestX == 0 && DestY == 0) {
		for (uint i = 0; i < SrcW * SrcH; i++) {
			byte Alpha = Source->Buffer[i].A;
			if (Alpha != 0) {
				if (Alpha == 255) {
					Dest->Buffer[i] = Source->Buffer[i];
					continue;
				}
				Dest->Buffer[i].R = OVERBLEND(Source->Buffer[i].R, Dest->Buffer[i].R, Alpha);
				Dest->Buffer[i].G = OVERBLEND(Source->Buffer[i].G, Dest->Buffer[i].G, Alpha);
				Dest->Buffer[i].B = OVERBLEND(Source->Buffer[i].B, Dest->Buffer[i].B, Alpha);
			}
		}
	} else {
		for (uint Y = 0; Y < SrcH; Y++) {
			for (uint X = 0; X < SrcW; X++) {
				if ((X + DestX >= Dest->Width) || (Y + DestY >= Dest->Height))
					continue;

				uint SrcOffset = Y * SrcW + X;
				uint DstOffset = (Y + DestY) * DstW + (X + DestX);

				byte Alpha = Source->Buffer[SrcOffset].A;
				if (Alpha != 0) {
					if (Alpha == 255) {
						Dest->Buffer[DstOffset] = Source->Buffer[SrcOffset];
						continue;
					}

					Dest->Buffer[DstOffset].R = OVERBLEND(Source->Buffer[SrcOffset].R, Dest->Buffer[DstOffset].R, Alpha);
					Dest->Buffer[DstOffset].G = OVERBLEND(Source->Buffer[SrcOffset].G, Dest->Buffer[DstOffset].G, Alpha);
					Dest->Buffer[DstOffset].B = OVERBLEND(Source->Buffer[SrcOffset].B, Dest->Buffer[DstOffset].B, Alpha);
				}
			}
		}
	}
}

void fgui_CanvasClear(FishGUI* GUI, FishCanvas* Canvas, byte R, byte G, byte B, byte A) {
	UNREF_PARAM(GUI);

	uint Len = Canvas->Height * Canvas->Width;
	FishPixel Pix = { R, G, B, A };

	for (uint i = 0; i < Len; i++)
		Canvas->Buffer[i] = Pix;
}

void fgui_CanvasResize(FishGUI* GUI, FishCanvas* Canvas, int W, int H) {
	FishCanvas* NewCanvas = fgui_CanvasCreate(GUI, W, H);

	uint SrcH = Canvas->Height;
	uint SrcW = Canvas->Width;
	uint DstH = H;
	uint DstW = W;

	for (uint Y = 0; Y < DstH; Y++) {
		uint SrcY = LERP(0, SrcH, Y, DstH);

		for (uint X = 0; X < DstW; X++) {
			uint SrcX = LERP(0, SrcW, X, DstW);

			uint DstOffset = Y * DstW + X;
			uint SrcOffset = SrcY * SrcW + SrcX;

			NewCanvas->Buffer[DstOffset] = Canvas->Buffer[SrcOffset];
		}
	}

	Canvas->Width = W;
	Canvas->Height = H;

	FGUI_FREE(Canvas->Buffer);
	Canvas->Buffer = NewCanvas->Buffer;
	NewCanvas->Buffer = NULL;

	fgui_CanvasDestroy(GUI, NewCanvas);
}

// Joke function :3
void fgui_CanvasDrawCanvasDepth(FishGUI* GUI, FishCanvas* Source, FishCanvas* SourceDepth,
	FishCanvas* Dest, FishCanvas* DestDepth, int DestX, int DestY) {

	UNREF_PARAM(GUI);

	uint SrcW = Source->Width;
	uint SrcH = Source->Height;
	uint DstW = Dest->Width;
	//uint DstH = Dest->Height;

	for (uint Y = 0; Y < SrcH; Y++) {
		for (uint X = 0; X < SrcW; X++) {
			uint SrcOffset = Y * SrcW + X;
			uint DstOffset = (Y + DestY) * DstW + (X + DestX);

			byte SrcD = SourceDepth->Buffer[SrcOffset].R;
			byte DstD = DestDepth->Buffer[DstOffset].R;

			if (SrcD >= DstD) {
				byte Alpha = Source->Buffer[SrcOffset].A;
				if (Alpha != 0) {
					Dest->Buffer[DstOffset].R = OVERBLEND(Source->Buffer[SrcOffset].R, Dest->Buffer[DstOffset].R, Alpha);
					Dest->Buffer[DstOffset].G = OVERBLEND(Source->Buffer[SrcOffset].G, Dest->Buffer[DstOffset].G, Alpha);
					Dest->Buffer[DstOffset].B = OVERBLEND(Source->Buffer[SrcOffset].B, Dest->Buffer[DstOffset].B, Alpha);
				}
			}
		}
	}
}