using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Drawing;
using System.Drawing.Imaging;
using System.Web;
using System.IO;
using static Test.FGUI;

namespace Test {
	class Program {
		static void Main(string[] args) {
			Bitmap Bmp = new Bitmap(800, 600);
			Graphics Gfx = Graphics.FromImage(Bmp);
			Gfx.Clear(Color.FromArgb(255, 255, 255, 255));
			Gfx.Flush();
			Gfx.Dispose();

			BitmapData Dta = Bmp.LockBits(new Rectangle(0, 0, Bmp.Width, Bmp.Height),
				ImageLockMode.ReadWrite, PixelFormat.Format32bppArgb);
			{
				IntPtr GUI = fgui_Init(Marshal.AllocHGlobal, Marshal.FreeHGlobal, Bmp.Width, Bmp.Height);

				IntPtr TestCanvas = fgui_CanvasCreate(GUI, 100, 100);

				fgui_CanvasClear(GUI, TestCanvas, 255, 0, 0, 255);
				fgui_DrawCanvas(GUI, TestCanvas, 100, 100);

				fgui_CanvasClear(GUI, TestCanvas, 0, 255, 0, 255);
				fgui_DrawCanvas(GUI, TestCanvas, 120, 120);

				fgui_CanvasClear(GUI, TestCanvas, 0, 0, 255, 255);
				//fgui_CanvasResize(GUI, TestCanvas, 200, 200);
				fgui_DrawCanvas(GUI, TestCanvas, 140, 140);

				byte[] PotatoBytes = File.ReadAllBytes("potato.png");
				IntPtr PotatoPtr = Marshal.AllocHGlobal(PotatoBytes.Length);
				Marshal.Copy(PotatoBytes, 0, PotatoPtr, PotatoBytes.Length);

				IntPtr Potato = fgui_CanvasCreateFromImage(GUI, PotatoPtr, PotatoBytes.Length);
				fgui_DrawCanvas(GUI, Potato, 0, 0);

				fgui_DrawScreenTo(GUI, Dta.Scan0, 2, 1, 0, 1, Dta.Stride - Dta.Width * 4);

			}
			Bmp.UnlockBits(Dta);
			Bmp.Save("test.png");

			Console.WriteLine("Done!");
			Console.ReadLine();
		}
	}
}