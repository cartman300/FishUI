using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.Numerics;

namespace Test {
	[UnmanagedFunctionPointer(FishGUI.DelegateCConv)]
	public delegate IntPtr FishAllocFunc(int Size);

	[UnmanagedFunctionPointer(FishGUI.DelegateCConv)]
	public delegate IntPtr FishReAllocFunc(IntPtr Ptr, int Size);

	[UnmanagedFunctionPointer(FishGUI.DelegateCConv)]
	public delegate void FishFreeFunc(IntPtr Ptr);

	public struct fgl_vec2 {
		public float x, y;

		public fgl_vec2(float x, float y) {
			this.x = x;
			this.y = y;
		}
	}

	public struct fgl_vec3 {
		public float x, y, z;

		public fgl_vec3(float x, float y, float z) {
			this.x = x;
			this.y = y;
			this.z = z;
		}
	}

	public struct fgl_triangle {
		public fgl_vec3 A, B, C;
		public fgl_vec2 A_UV, B_UV, C_UV;

		public fgl_triangle(fgl_vec3 A, fgl_vec3 B, fgl_vec3 C, fgl_vec2 A_UV, fgl_vec2 B_UV, fgl_vec2 C_UV) {
			this.A = A;
			this.B = B;
			this.C = C;
			this.A_UV = A_UV;
			this.B_UV = B_UV;
			this.C_UV = C_UV;
		}
	}

	[StructLayout(LayoutKind.Explicit)]
	public struct fgl_color {
		public static fgl_color Black = new fgl_color(0, 0, 0, 255);
		public static fgl_color White = new fgl_color(255, 255, 255, 255);
		public static fgl_color DepthZero = new fgl_color(0);

		[FieldOffset(0)]
		public byte R;

		[FieldOffset(1)]
		public byte G;

		[FieldOffset(2)]
		public byte B;

		[FieldOffset(3)]
		public byte A;

		[FieldOffset(0)]
		public float Float;

		public fgl_color(byte R, byte G, byte B, byte A) {
			this.Float = 0;
			this.R = R;
			this.G = G;
			this.B = B;
			this.A = A;
		}

		public fgl_color(float Float) : this(0, 0, 0, 0) {
			this.Float = Float;
		}
	}

	public unsafe class FishGUI {
		const string DllName = "FishUI";
		const CallingConvention CConv = CallingConvention.Cdecl;
		public const CallingConvention DelegateCConv = CallingConvention.Cdecl;

		static GCHandle AllocHandle, ReAllocHandle, FreeHandle;

		[DllImport(DllName, CallingConvention = CConv)]
		public static extern void fish_Init(FishAllocFunc Alloc, FishReAllocFunc ReAlloc, FishFreeFunc Free);

		public static void fish_Init() {
			FishAllocFunc Alloc = Marshal.AllocHGlobal;
			FishReAllocFunc ReAlloc = (Ptr, Sz) => {
				if (Ptr == IntPtr.Zero)
					return Alloc(Sz);
				return Marshal.ReAllocHGlobal(Ptr, (IntPtr)Sz);
			};
			FishFreeFunc Free = Marshal.FreeHGlobal;

			AllocHandle = GCHandle.Alloc(Alloc);
			ReAllocHandle = GCHandle.Alloc(ReAlloc);
			FreeHandle = GCHandle.Alloc(Free);

			fish_Init(Alloc, ReAlloc, Free);
		}

		[DllImport(DllName, CallingConvention = CConv)]
		public static extern void INIT_REST();

		[DllImport(DllName, CallingConvention = CConv)]
		public static extern IntPtr fgl_CreateTriangleBuffer();

		[DllImport(DllName, CallingConvention = CConv)]
		public static extern void fgl_DestroyTriangleBuffer(IntPtr Buffer);

		[DllImport(DllName, CallingConvention = CConv)]
		public static extern void fgl_TriangleBufferData(IntPtr Buffer, fgl_triangle[] Triangles, int Count);

		[DllImport(DllName, CallingConvention = CConv)]
		public static extern IntPtr fgl_CreateFramebuffer(int Width, int Height);

		[DllImport(DllName, CallingConvention = CConv)]
		public static extern IntPtr fgl_CreateFramebufferFromImage(byte[] Data, int Len);

		[DllImport(DllName, CallingConvention = CConv)]
		public static extern void fgl_DestroyFramebuffer(IntPtr Framebuffer);

		[DllImport(DllName, CallingConvention = CConv)]
		public static extern IntPtr fgl_GetFramebufferData(IntPtr Framebuffer, out int Len);

		[DllImport(DllName, CallingConvention = CConv)]
		public static extern fgl_color fgl_SampleFramebuffer(IntPtr Framebuffer, int X, int Y);

		[DllImport(DllName, CallingConvention = CConv)]
		public static extern void fgl_ClearFramebuffer(IntPtr Framebuffer, fgl_color Clr);

		[DllImport(DllName, CallingConvention = CConv)]
		public static extern void fgl_BindColorBuffer(IntPtr Framebuffer);

		[DllImport(DllName, CallingConvention = CConv)]
		public static extern void fgl_BindDepthBuffer(IntPtr Framebuffer);

		[DllImport(DllName, CallingConvention = CConv)]
		public static extern void fgl_BindTexture(IntPtr Framebuffer, int TextureSlot);

		[DllImport(DllName, CallingConvention = CConv)]
		public static extern void fgl_SetDrawColor(fgl_color Clr);

		[DllImport(DllName, CallingConvention = CConv)]
		public static extern void fgl_DrawTriangleBuffer(IntPtr Buffer);

		[DllImport(DllName, CallingConvention = CConv)]
		public static extern void fgl_EnableWireframe(int Enable);

		[DllImport(DllName, CallingConvention = CConv)]
		public static extern void fgl_EnableDepthTesting(int Enable);

		[DllImport(DllName, CallingConvention = CConv)]
		public static extern void fgl_EnableBackfaceCulling(int Enable);

		[DllImport(DllName, CallingConvention = CConv)]
		public static extern void fgl_SetModelMatrix(Matrix4x4* ModelMatrix);

		public static void fgl_SetModelMatrix(Matrix4x4 ModelMatrix) {
			fgl_SetModelMatrix(&ModelMatrix);
		}

		[DllImport(DllName, CallingConvention = CConv)]
		public static extern void fgl_SetViewMatrix(Matrix4x4* ViewMatrix);

		public static void fgl_SetViewMatrix(Matrix4x4 ViewMatrix) {
			fgl_SetViewMatrix(&ViewMatrix);
		}

		[DllImport(DllName, CallingConvention = CConv)]
		public static extern void fgl_SetProjectionMatrix(Matrix4x4* ProjectionMatrix);

		public static void fgl_SetProjectionMatrix(Matrix4x4 ProjectionMatrix) {
			fgl_SetProjectionMatrix(&ProjectionMatrix);
		}

		[DllImport(DllName, CallingConvention = CConv)]
		public static extern IntPtr fgl_CopyFramebuffer(IntPtr Src);

		[DllImport(DllName, CallingConvention = CConv)]
		public static extern void fgl_CopyFramebufferData(IntPtr Src, IntPtr Dest);
	}
}
