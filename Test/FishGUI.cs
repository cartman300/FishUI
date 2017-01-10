using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

namespace Test {
	[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
	public delegate IntPtr FishAllocFunc(int Size);

	[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
	public delegate void FishFreeFunc(IntPtr Ptr);

	public class FGUI {
		const string DllName = "FishUI";

		[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr fgui_Init(FishAllocFunc Alloc, FishFreeFunc Free, int Width, int Height);

		[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void fgui_SetResolution(IntPtr GUI, int Width, int Height);

		[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void fgui_DrawCanvas(IntPtr GUI, IntPtr Canvas, int X, int Y);

		[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void fgui_DrawScreenTo(IntPtr GUI, IntPtr Output, int RedOffset, int GreenOffset, int BlueOffset, int PixelStride, int LineStride);

		// Canvas

		[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr fgui_CanvasCreate(IntPtr GUI, int W, int H);

		[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr fgui_CanvasCreateFromImage(IntPtr GUI, IntPtr Data, int Len);

		[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void fgui_CanvasDestroy(IntPtr GUI, IntPtr Canvas);

		[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void fgui_CanvasDrawCanvas(IntPtr GUI, IntPtr Source, IntPtr Dest, int DestX, int DestY);

		[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void fgui_CanvasClear(IntPtr GUI, IntPtr Canvas, byte R, byte G, byte B, byte A);

		[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void fgui_CanvasResize(IntPtr GUI, IntPtr Canvas, int W, int H);
	}
}
