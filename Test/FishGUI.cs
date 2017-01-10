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
		public static extern void fgui_Init(IntPtr GUI, FishAllocFunc Alloc, FishFreeFunc Free, int Width, int Height);

		[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void fgui_SetResolution(IntPtr GUI, int Width, int Height);

		[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void fgui_DrawCanvas(IntPtr GUI, IntPtr Canvas, int X, int Y);

		[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void fgui_DrawScreenTo(IntPtr GUI, IntPtr Output, int RedOffset, int GreenOffset, int BlueOffset, int PixelStride, int LineStride);
	}
}
