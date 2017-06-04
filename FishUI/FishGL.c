#include "stdafx.h"

#define FISHGL_IMPL
#include "FishGL.h"

#include "stb_image.h"

// Vectors, matrices and all that shit
#include "linmath.h"

#define _X 0
#define _Y 1
#define _Z 2
#define _W 3

#define __OVERBLEND(fg, bg, alpha) (((fg * alpha) + (bg * (255 - alpha))) / 255)
#define OVERBLEND(fg, bg, alpha) __OVERBLEND((fg), (bg), (alpha))

// State functions

void fgl_EnableWireframe(int Enable) {
	fgl_state.Wireframe = (bool)Enable;
}

void fgl_EnableDepthTesting(int Enable) {
	fgl_state.DepthTesting = (bool)Enable;
}

void fgl_EnableBackfaceCulling(int Enable) {
	fgl_state.BackfaceCulling = (bool)Enable;
}

void fgl_SetDrawColor(fgl_color Clr) {
	fgl_state.DrawColor = Clr;
}

static inline void fgl_UpdateMVSP() {
	mat4x4_mul(fgl_state.VertexShaderMatrix, fgl_state.ProjectionMatrix, fgl_state.ScreenMatrix);
	mat4x4_mul(fgl_state.VertexShaderMatrix, fgl_state.VertexShaderMatrix, fgl_state.ViewMatrix);
	mat4x4_mul(fgl_state.VertexShaderMatrix, fgl_state.VertexShaderMatrix, fgl_state.ModelMatrix);
}

void fgl_BindColorBuffer(fgl_framebuffer* Framebuffer) {
	fgl_state.ColorBuffer = Framebuffer;

	float W = (float)Framebuffer->Width;
	float H = (float)Framebuffer->Height;

	mat4x4 Scr;
	mat4x4_identity(Scr);
	/*mat4x4_translate(Scr, (W * (W / H)) / 2, H / 2, -1);
	mat4x4_scale_aniso(Scr, Scr, W / 2, H / 2, -1);*/
	memcpy(fgl_state.ScreenMatrix, Scr, sizeof(mat4x4));

	fgl_UpdateMVSP();
}

void fgl_BindDepthBuffer(fgl_framebuffer* Framebuffer) {
	fgl_state.DepthBuffer = Framebuffer;
}

void fgl_BindTexture(fgl_framebuffer* Framebuffer, int TextureSlot) {
	fgl_state.TextureBuffers[TextureSlot] = Framebuffer;
}

void fgl_SetModelMatrix(mat4x4 ModelMatrix) {
	memcpy(fgl_state.ModelMatrix, ModelMatrix, sizeof(mat4x4));
	fgl_UpdateMVSP();
}

void fgl_SetViewMatrix(mat4x4 ViewMatrix) {
	memcpy(fgl_state.ViewMatrix, ViewMatrix, sizeof(mat4x4));
	fgl_UpdateMVSP();
}

void fgl_SetProjectionMatrix(mat4x4 ProjectionMatrix) {
	memcpy(fgl_state.ProjectionMatrix, ProjectionMatrix, sizeof(mat4x4));
	fgl_UpdateMVSP();
}

// Triangle buffers

fgl_triangle_buffer* fgl_CreateTriangleBuffer() {
	fgl_triangle_buffer* Buffer = FGUI_ALLOC(sizeof(fgl_triangle_buffer));
	Buffer->Triangles = NULL;
	Buffer->Length = 0;
	return Buffer;
}

void fgl_DestroyTriangleBuffer(fgl_triangle_buffer* Buffer) {
	if (Buffer->Triangles != NULL)
		FGUI_FREE(Buffer->Triangles);
	FGUI_FREE(Buffer);
}

void fgl_TriangleBufferData(fgl_triangle_buffer* Buffer, fgl_triangle* Triangles, int Count) {
	if (Buffer->Triangles != NULL)
		FGUI_FREE(Buffer->Triangles);

	Buffer->Triangles = FGUI_ALLOC(sizeof(fgl_triangle) * Count);
	Buffer->Length = Count;

	memcpy(Buffer->Triangles, Triangles, sizeof(fgl_triangle) * Count);
}

// Framebuffers

fgl_framebuffer* fgl_CreateFramebuffer(int Width, int Height) {
	fgl_framebuffer* Framebuffer = FGUI_ALLOC(sizeof(fgl_framebuffer));
	Framebuffer->Width = Width;
	Framebuffer->Height = Height;
	Framebuffer->Length = Width * Height;
	Framebuffer->Data = FGUI_ALLOC(Framebuffer->Length * sizeof(fgl_color));
	return Framebuffer;
}

fgl_framebuffer* fgl_CreateFramebufferFromImage(byte* Data, int Len) {
	int W, H, Channels;
	byte* ImgData = stbi_load_from_memory(Data, Len, &W, &H, &Channels, 4);

	fgl_framebuffer* Framebuffer = fgl_CreateFramebuffer(W, H);
	memcpy(Framebuffer->Data, ImgData, W * H * Channels);

	stbi_image_free(ImgData);
	return Framebuffer;
}

fgl_framebuffer* fgl_CopyFramebuffer(fgl_framebuffer* Src) {
	fgl_framebuffer* Framebuffer = fgl_CreateFramebuffer(Src->Width, Src->Height);
	fgl_CopyFramebufferData(Src, Framebuffer);
	return Framebuffer;
}

void fgl_CopyFramebufferData(fgl_framebuffer* Src, fgl_framebuffer* Dest) {
	for (uint i = 0; i < Src->Length; i++)
		Dest->Data[i] = Src->Data[i];
}

void fgl_DestroyFramebuffer(fgl_framebuffer* Framebuffer) {
	FGUI_FREE(Framebuffer->Data);
	FGUI_FREE(Framebuffer);
}

void fgl_ClearFramebuffer(fgl_framebuffer* Framebuffer, fgl_color Clr) {
	memset32(Framebuffer->Data, Clr.Integer, Framebuffer->Length);
}

fgl_color* fgl_GetFramebufferData(fgl_framebuffer* Framebuffer, int** Len) {
	if (Len != NULL)
		*Len = Framebuffer->Length * sizeof(fgl_color);
	return Framebuffer->Data;
}

fgl_color fgl_SampleFramebuffer(fgl_framebuffer* Framebuffer, int X, int Y) {
	if (X < 0 || Y < 0 || X > Framebuffer->Width || Y > Framebuffer->Height) {
		fgl_color Empty;
		Empty.Integer = 0;
		return Empty;
	}

	return Framebuffer->Data[Y * Framebuffer->Width + X];
}

// Triangle drawing
static inline void fgl_BoundingBox(fgl_triangle* Tri, vec3 min, vec3 max) {
	min[0] = fminf(fminf(Tri->A.x, Tri->B.x), Tri->C.x);
	min[1] = fminf(fminf(Tri->A.y, Tri->B.y), Tri->C.y);
	min[2] = fminf(fminf(Tri->A.z, Tri->B.z), Tri->C.z);

	max[0] = fmaxf(fmaxf(Tri->A.x, Tri->B.x), Tri->C.x);
	max[1] = fmaxf(fmaxf(Tri->A.y, Tri->B.y), Tri->C.y);
	max[2] = fmaxf(fmaxf(Tri->A.z, Tri->B.z), Tri->C.z);
}

static inline void fgl_Barycentric(fgl_triangle* T, int PX, int PY, vec3 Val) {
	vec3 U, a, b;

	a[0] = (T->C.x) - (T->A.x);
	a[1] = (T->B.x) - (T->A.x);
	a[2] = (T->A.x) - (float)PX;

	b[0] = (T->C.y) - (T->A.y);
	b[1] = (T->B.y) - (T->A.y);
	b[2] = (T->A.y) - (float)PY;

	vec3_mul_cross(U, a, b);

	if (fabsf(U[_Z]) < 1) {
		Val[0] = -1;
		return;
	}

	Val[_X] = 1.0f - ((U[_X] + U[_Y]) / U[_Z]);
	Val[_Y] = U[_Y] / U[_Z];
	Val[_Z] = U[_X] / U[_Z];
}

static inline void fgl_VertexProcessor(vec3 VertPos, bool* Discard) {
	vec4 Pt;
	memcpy(Pt, VertPos, sizeof(vec3));
	Pt[_W] = 1.0f;

	vec4 Res;
	mat4x4_mul_vec4(Res, fgl_state.VertexShaderMatrix, Pt);

	if (Res[_X] > 1 || Res[_X] < -1 || Res[_Y] > 1 || Res[_Y] < -1 || Res[_Z] > 1 || Res[_Z] < -1 || Res[_W] > 1 || Res[_W] < -1)
		*Discard = true;

	memcpy(VertPos, Res, sizeof(vec3));

	vec3 offset = { 1, 1, -1 };
	vec4 scale = { 0, 0, -1, 1 };
	vec3_add(VertPos, VertPos, offset);
	scale[_X] = (float)fgl_state.ColorBuffer->Width / 2;
	scale[_Y] = (float)fgl_state.ColorBuffer->Height / 2;
	vec3_scale_vec(VertPos, VertPos, scale);
}

static inline fgl_color fgl_PixelProcessor(float U, float V, float Depth, vec3 cross, bool* Discard) {
	U = fmaxf(fminf(U, 1), 0);
	V = fmaxf(fminf(V, 1), 0);

	U *= (fgl_state.TextureBuffers[0]->Width - 1);
	V *= (fgl_state.TextureBuffers[0]->Height - 1);

	fgl_color Clr = fgl_state.TextureBuffers[0]->Data[(int)V * fgl_state.TextureBuffers[0]->Width + (int)U];

	*Discard = Clr.A == 0 ? true : false;
	//*Discard = false;
	return Clr;
}

void fgl_DrawTriangle(fgl_triangle Tri) {
	bool Discard0 = false, Discard1 = false, Discard2 = false;

	fgl_VertexProcessor((float*)&Tri.A, &Discard0);
	fgl_VertexProcessor((float*)&Tri.B, &Discard1);
	fgl_VertexProcessor((float*)&Tri.C, &Discard2);

	if (Discard0 && Discard1 && Discard2)
		return;

	vec3 cross, min, max, bcentric;
	vec3_sub(min, (float*)&Tri.C, (float*)&Tri.A);
	vec3_sub(max, (float*)&Tri.B, (float*)&Tri.A);
	vec3_mul_cross(cross, min, max);
	vec3_norm(cross, cross);

	// Backface culling
	if (fgl_state.BackfaceCulling && cross[_Z] > 0)
		return;

	fgl_BoundingBox(&Tri, min, max);

	for (int Y = min[_Y]; Y < max[_Y]; Y++)
		for (int X = min[_X]; X < max[_X]; X++) {
			if (X < 0 || Y < 0 || X >= fgl_state.ColorBuffer->Width || Y >= fgl_state.ColorBuffer->Height)
				continue;

			fgl_Barycentric(&Tri, X, Y, bcentric);
			if (bcentric[_X] < 0 || bcentric[_Y] < 0 || bcentric[_Z] < 0)
				continue;

			float D = ((Tri.A.z * bcentric[_X]) + (Tri.B.z * bcentric[_Y]) + (Tri.C.z * bcentric[_Z]));
			int Idx = Y * fgl_state.ColorBuffer->Width + X;

			if (!fgl_state.DepthTesting || fgl_state.DepthBuffer->Data[Idx].Float < D) {
				// Calculate UV coordinates
				float TexU = ((Tri.UVs.A.x * bcentric[_X]) + (Tri.UVs.B.x * bcentric[_Y]) + (Tri.UVs.C.x * bcentric[_Z]));
				float TexV = 1.0f - ((Tri.UVs.A.y * bcentric[_X]) + (Tri.UVs.B.y * bcentric[_Y]) + (Tri.UVs.C.y * bcentric[_Z]));

				bool DiscardPixel = false;
				fgl_color PixColor = fgl_PixelProcessor(TexU, TexV, D, cross, &DiscardPixel);
				if (DiscardPixel)
					continue;

				fgl_state.DepthBuffer->Data[Idx].Float = D;

				if (PixColor.A = 0)
					continue;
				else if (PixColor.A = 255) {
					fgl_state.ColorBuffer->Data[Idx] = PixColor;
				}
				else {
					fgl_color* Dst = &fgl_state.ColorBuffer->Data[Idx];
					Dst->R = OVERBLEND(PixColor.R, Dst->R, PixColor.A);
					Dst->G = OVERBLEND(PixColor.R, Dst->R, PixColor.A);
					Dst->B = OVERBLEND(PixColor.R, Dst->R, PixColor.A);
				}

			}
		}

#define DRAW_LINE(A, B) fgl_DrawLine(A.x, A.y, B.x, B.y)
	if (fgl_state.Wireframe) {
		DRAW_LINE(Tri.A, Tri.B);
		DRAW_LINE(Tri.B, Tri.C);
		DRAW_LINE(Tri.C, Tri.A);
	}
#undef DRAW_LINE
}

void fgl_DrawTriangleBuffer(fgl_triangle_buffer* Buffer) {
	for (uint i = 0; i < Buffer->Length; i++)
		fgl_DrawTriangle(Buffer->Triangles[i]);
}

void fgl_DrawLine(int X0, int Y0, int X1, int Y1) {
	bool Steep = false;

	if (abs(X0 - X1) < abs(Y0 - Y1)) {
		int Tmp = X0;
		X0 = Y0;
		Y0 = Tmp;

		Tmp = X1;
		X1 = Y1;
		Y1 = Tmp;

		Steep = true;
	}

	if (X0 > X1) {
		int Tmp = X0;
		X0 = X1;
		X1 = Tmp;

		Tmp = Y0;
		Y0 = Y1;
		Y1 = Tmp;
	}

	int DeltaX = X1 - X0;
	int DeltaY = Y1 - Y0;
	int DeltaError2 = abs(DeltaY) * 2;
	int Error2 = 0;
	int Y = Y0;

	for (int X = X0; X <= X1; X++) {
		if (Steep) {
			if (X < 0 || X >= fgl_state.ColorBuffer->Height || Y < 0 || Y >= fgl_state.ColorBuffer->Width)
				continue;

			fgl_state.ColorBuffer->Data[X * fgl_state.ColorBuffer->Width + Y] = fgl_state.DrawColor;
		}
		else {
			if (Y < 0 || Y >= fgl_state.ColorBuffer->Height || X < 0 || X >= fgl_state.ColorBuffer->Width)
				continue;

			fgl_state.ColorBuffer->Data[Y * fgl_state.ColorBuffer->Width + X] = fgl_state.DrawColor;
		}

		Error2 += DeltaError2;

		if (Error2 > DeltaX) {
			Y += (Y1 > Y0 ? 1 : -1);
			Error2 -= DeltaX * 2;
		}
	}
}