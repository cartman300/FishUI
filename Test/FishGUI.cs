using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

namespace Test {
	[UnmanagedFunctionPointer(FGUI.DelegateCConv)]
	public delegate IntPtr FishAllocFunc(int Size);

	[UnmanagedFunctionPointer(FGUI.DelegateCConv)]
	public delegate IntPtr FishReAllocFunc(IntPtr Ptr, int Size);

	[UnmanagedFunctionPointer(FGUI.DelegateCConv)]
	public delegate void FishFreeFunc(IntPtr Ptr);

	public class FGUI {
		const string DllName = "FishUI";
		const CallingConvention CConv = CallingConvention.Cdecl;
		public const CallingConvention DelegateCConv = CallingConvention.Cdecl;

		[DllImport(DllName, CallingConvention = CConv)]
		public static extern IntPtr fgui_Init(FishAllocFunc Alloc, FishReAllocFunc ReAlloc, FishFreeFunc Free, int Width, int Height);

		public static IntPtr fgui_InitSafe(FishAllocFunc Alloc, FishReAllocFunc ReAlloc, FishFreeFunc Free, int Width, int Height) {
			GCHandle.Alloc(Alloc, GCHandleType.Normal);
			GCHandle.Alloc(ReAlloc, GCHandleType.Normal);
			GCHandle.Alloc(Free, GCHandleType.Normal);
			return fgui_Init(Alloc, ReAlloc, Free, Width, Height);
		}

		[DllImport(DllName, CallingConvention = CConv)]
		public static extern void fgui_SetResolution(IntPtr GUI, int Width, int Height);

		[DllImport(DllName, CallingConvention = CConv)]
		public static extern IntPtr fgui_GetScreenCanvas(IntPtr GUI);

		[DllImport(DllName, CallingConvention = CConv)]
		public static extern void fgui_DrawScreenTo(IntPtr GUI, IntPtr Output, int RedOffset, int GreenOffset, int BlueOffset, int PixelStride, int LineStride);

		// Canvas

		[DllImport(DllName, CallingConvention = CConv)]
		public static extern IntPtr fgui_CanvasCreate(IntPtr GUI, int W, int H);

		[DllImport(DllName, CallingConvention = CConv)]
		public static extern IntPtr fgui_CanvasCreateFromImage(IntPtr GUI, IntPtr Data, int Len);

		[DllImport(DllName, CallingConvention = CConv)]
		public static extern void fgui_CanvasDestroy(IntPtr GUI, IntPtr Canvas);

		[DllImport(DllName, CallingConvention = CConv)]
		public static extern void fgui_CanvasDrawCanvas(IntPtr GUI, IntPtr Source, IntPtr Dest, int DestX, int DestY);

		[DllImport(DllName, CallingConvention = CConv)]
		public static extern void fgui_CanvasClear(IntPtr GUI, IntPtr Canvas, byte R, byte G, byte B, byte A);

		[DllImport(DllName, CallingConvention = CConv)]
		public static extern void fgui_CanvasResize(IntPtr GUI, IntPtr Canvas, int W, int H);

		[DllImport(DllName, CallingConvention = CConv)]
		public static extern void fgui_CanvasDrawCanvasDepth(IntPtr GUI, IntPtr Source, IntPtr SourceDepth,
			IntPtr Dest, IntPtr DestDepth, int DestX, int DestY);
	}
}
