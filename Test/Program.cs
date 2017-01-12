using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Drawing;
using System.Drawing.Imaging;
using System.Web;
using System.Threading;
using System.Diagnostics;
using System.IO;
using System.Windows.Forms;

using static Test.FGUI;

namespace Test {
	class RenderForm : Form {
		Stopwatch SWatch;

		public RenderForm() {
			SWatch = new Stopwatch();

			Text = "Hurr Durr";
			ClientSize = new Size(800, 600);
			StartPosition = FormStartPosition.CenterScreen;
			FormBorderStyle = FormBorderStyle.Fixed3D;
			MaximizeBox = false;
			DoubleBuffered = true;
		}

		protected override void OnPaint(PaintEventArgs e) {
			base.OnPaint(e);

			e.Graphics.Clear(Color.CornflowerBlue);

			BitmapData Dta = Program.Bmp.LockBits(new Rectangle(0, 0, Program.Bmp.Width, Program.Bmp.Height),
				ImageLockMode.ReadWrite, PixelFormat.Format32bppArgb);

			SWatch.Restart();
			Program.Rendurr(Dta);
			SWatch.Stop();

			Program.Bmp.UnlockBits(Dta);

			e.Graphics.DrawImage(Program.Bmp, 0, 0);
			e.Graphics.DrawString(string.Format("Time: {0} ms", SWatch.ElapsedMilliseconds),
				SystemFonts.StatusFont, Brushes.White, 0, 0);
		}
	}

	class Program {
		static IntPtr GUI = IntPtr.Zero;
		public static Bitmap Bmp;

		static IntPtr PotatoPtr, PotatoDepthPtr;
		static int PotatoLen, PotatoDepthLen;

		static IntPtr PotatoCanvas = IntPtr.Zero, TestCanvas = IntPtr.Zero;

		static IntPtr ReAlloc(IntPtr Ptr, int Size) {
			if (Ptr == IntPtr.Zero)
				return Marshal.AllocHGlobal(Size);
			return Marshal.ReAllocHGlobal(Ptr, (IntPtr)Size);
		}

		public static void Rendurr(BitmapData Dta) {
			if (GUI == IntPtr.Zero)
				GUI = fgui_InitSafe(Marshal.AllocHGlobal, ReAlloc, Marshal.FreeHGlobal, Bmp.Width, Bmp.Height);

			IntPtr ScreenCanvas = fgui_GetScreenCanvas(GUI);
			fgui_CanvasClear(GUI, ScreenCanvas, 54, 57, 62, 255);

			if (TestCanvas == IntPtr.Zero)
				TestCanvas = fgui_CanvasCreate(GUI, 100, 100);
			fgui_CanvasClear(GUI, TestCanvas, 255, 0, 0, 255);
			fgui_CanvasDrawCanvas(GUI, TestCanvas, ScreenCanvas, 100, 100);

			fgui_CanvasClear(GUI, TestCanvas, 0, 255, 0, 255);
			fgui_CanvasDrawCanvas(GUI, TestCanvas, ScreenCanvas, 120, 120);

			fgui_CanvasClear(GUI, TestCanvas, 0, 0, 255, 255);
			//fgui_CanvasResize(GUI, TestCanvas, 200, 200);
			fgui_CanvasDrawCanvas(GUI, TestCanvas, ScreenCanvas, 140, 140);
			//fgui_CanvasDestroy(GUI, TestCanvas);

			if (PotatoCanvas == IntPtr.Zero)
				PotatoCanvas = fgui_CanvasCreateFromImage(GUI, PotatoPtr, PotatoLen);
			/*(int)(400 * Math.Sin((double)SWatch.ElapsedMilliseconds / 2000))*/
			fgui_CanvasDrawCanvas(GUI, PotatoCanvas, ScreenCanvas, 0, 0);
			//fgui_CanvasDestroy(GUI, PotatoCanvas);

			fgui_DrawScreenTo(GUI, Dta.Scan0, 2, 1, 0, 1, Dta.Stride - Dta.Width * 4);
		}

		static RenderForm RF;
		static Stopwatch SWatch;

		static void Main(string[] args) {
			byte[] PotatoBytes = File.ReadAllBytes("potato.png");
			PotatoPtr = Marshal.AllocHGlobal(PotatoBytes.Length);
			Marshal.Copy(PotatoBytes, 0, PotatoPtr, PotatoBytes.Length);
			PotatoLen = PotatoBytes.Length;

			byte[] PotatoDepthBytes = File.ReadAllBytes("potato_heightmap.png");
			PotatoDepthPtr = Marshal.AllocHGlobal(PotatoDepthBytes.Length);
			Marshal.Copy(PotatoDepthBytes, 0, PotatoDepthPtr, PotatoDepthBytes.Length);
			PotatoDepthLen = PotatoDepthBytes.Length;

			Bmp = new Bitmap(800, 600);
			RF = new RenderForm();

			SWatch = new Stopwatch();
			SWatch.Start();

			using (Graphics Gfx = Graphics.FromImage(Bmp))
				Gfx.Clear(Color.FromArgb(255, 54, 57, 62));

			Thread Worker = new Thread(() => {
				Thread.Sleep(1000);
				Action RefreshAction = new Action(RF.Refresh);

				while (!RF.IsDisposed) {
					RF.Invoke(RefreshAction);
					Thread.Sleep(100);
				}
			});
			Worker.IsBackground = true;
			Worker.Start();

			Application.Run(RF);
			Environment.Exit(0);
		}
	}
}