using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Drawing;
using System.Drawing.Imaging;

using static Test.FGUI;

namespace Test {
	class Program {
		static void Main(string[] args) {
			IntPtr GUI = Marshal.AllocHGlobal(32);
			fgui_Init(GUI, Marshal.AllocHGlobal, Marshal.FreeHGlobal, 800, 600);

			using (Bitmap Bmp = new Bitmap(800, 600)) {
				BitmapData Dta = Bmp.LockBits(new Rectangle(0, 0, Bmp.Width, Bmp.Height),
					ImageLockMode.ReadWrite, PixelFormat.Format32bppArgb);

				fgui_DrawScreenTo(GUI, Dta.Scan0, 1, 2, 3, 1, Dta.Stride);

				Bmp.UnlockBits(Dta);
				Bmp.Save("test.png");
			}

			Console.WriteLine("Done!");
			Console.ReadLine();
		}
	}
}