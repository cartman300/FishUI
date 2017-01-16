#pragma once

#ifndef FISHGL_IMPL
#define FISHGL_EXTERN extern
#else
#define FISHGL_EXTERN
#endif

typedef struct {
	float x, y;
} fgl_vec2;

typedef struct {
	float x, y, z;
} fgl_vec3;

typedef struct {
	fgl_vec3 A, B, C;
	struct {
		fgl_vec2 A, B, C;
	} UVs;
} fgl_triangle;

typedef struct {
	fgl_triangle* Triangles;
	int Length;
} fgl_triangle_buffer;

typedef struct {
	union {
		struct {
			byte R, G, B, A;
		};

		int Integer;
		float Float;
	};
} fgl_color;

typedef struct {
	int Width;
	int Height;
	int Length;
	fgl_color* Data;
} fgl_framebuffer;

// State

FISHGL_EXTERN struct {
	float ModelMatrix[16];
	float ViewMatrix[16];
	float ProjectionMatrix[16];
	float ScreenMatrix[16];
	float VertexShaderMatrix[16];

	bool Wireframe;
	bool DepthTesting;

	fgl_color DrawColor;
	fgl_framebuffer* ColorBuffer;
	fgl_framebuffer* DepthBuffer;
	fgl_framebuffer* TextureBuffers[32];
} fgl_state;

// State functions
EXPORT void fgl_SetDrawColor(fgl_color Clr);
EXPORT void fgl_BindColorBuffer(fgl_framebuffer* Framebuffer);
EXPORT void fgl_BindDepthBuffer(fgl_framebuffer* Framebuffer);
EXPORT void fgl_BindTexture(fgl_framebuffer* Framebuffer, int TextureSlot);
EXPORT void fgl_EnableWireframe(int Enable);
EXPORT void fgl_EnableDepthTesting(int Enable);
EXPORT void fgl_SetModelMatrix(float* ModelMatrix);
EXPORT void fgl_SetViewMatrix(float* ViewMatrix);
EXPORT void fgl_SetProjectionMatrix(float* ProjectionMatrix);

// Drawing functions
EXPORT void fgl_DrawTriangleBuffer(fgl_triangle_buffer* Buffer);
EXPORT void fgl_DrawTriangle(fgl_triangle Tri);
EXPORT void fgl_DrawLine(int X0, int Y0, int X1, int Y1);

// Triangle buffers
EXPORT fgl_triangle_buffer* fgl_CreateTriangleBuffer();
EXPORT void fgl_DestroyTriangleBuffer(fgl_triangle_buffer* Buffer);
EXPORT void fgl_TriangleBufferData(fgl_triangle_buffer* Buffer, fgl_triangle* Triangles, int Count);

// Framebuffers
EXPORT fgl_framebuffer* fgl_CreateFramebuffer(int Width, int Height);
EXPORT fgl_framebuffer* fgl_CreateFramebufferFromImage(byte* Data, int Len);
EXPORT void fgl_DestroyFramebuffer(fgl_framebuffer* Framebuffer);
EXPORT void fgl_ClearFramebuffer(fgl_framebuffer* Framebuffer, fgl_color Clr);
EXPORT fgl_color* fgl_GetFramebufferData(fgl_framebuffer* Framebuffer, int** Len);
EXPORT fgl_framebuffer* fgl_CopyFramebuffer(fgl_framebuffer* Src);
EXPORT void fgl_CopyFramebufferData(fgl_framebuffer* Src, fgl_framebuffer* Dest);