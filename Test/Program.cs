using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
//using System.Drawing;
using System.Drawing.Imaging;
using System.Web;
using System.Threading;
using System.Diagnostics;
using System.IO;
using System.Windows.Forms;
using SFML.Graphics;
using SFML.System;
using SFML.Window;
using System.Numerics;

using static Test.FishGUI;

namespace Test {
	/*class RenderForm : Form {
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
			//(int)(400 * Math.Sin((double)SWatch.ElapsedMilliseconds / 2000))
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
	}*/

	unsafe class Program {
		static int W = 800;
		static int H = 600;
		static int TriangleCount;
		static byte[] FramebufferData;

		static IntPtr ColorBuffer, DepthBuffer, TextureBuffer;
		static IntPtr TriangleBuffer;

		static Stopwatch SWatch = Stopwatch.StartNew();

		static void Init() {
			fish_Init();
			fgl_EnableDepthTesting(1);
			//fgl_EnableWireframe(1);
			fgl_SetDrawColor(fgl_color.White);

			ColorBuffer = fgl_CreateFramebuffer(W, H);
			fgl_BindColorBuffer(ColorBuffer);

			DepthBuffer = fgl_CreateFramebuffer(W, H);
			fgl_BindDepthBuffer(DepthBuffer);

			fgl_SetModelMatrix(Matrix4x4.Identity);
			fgl_SetViewMatrix(Matrix4x4.Identity);
			fgl_SetProjectionMatrix(Matrix4x4.CreatePerspectiveFieldOfView(90 * (float)Math.PI / 180, (float)W / H, 0.001f, 100f));
			//fgl_SetProjectionMatrix(Matrix4x4.CreateOrthographicOffCenter(0, 1, 0, 1, 0.001f, 100f));

			// Load model
			fgl_triangle[] Triangles = ObjLoader.Load("models\\diablo3_pose\\diablo3_pose.obj");
			TriangleBuffer = fgl_CreateTriangleBuffer();
			fgl_TriangleBufferData(TriangleBuffer, Triangles, TriangleCount = Triangles.Length);//*/

			/*TriangleBuffer = fgl_CreateTriangleBuffer();
			fgl_triangle[] Triangles = new fgl_triangle[] {
				new fgl_triangle(new fgl_vec3(0, 0, 0), new fgl_vec3(0, 1, 0), new fgl_vec3(1, 1, 0),
								 new fgl_vec2(0, 0), new fgl_vec2(0, 1), new fgl_vec2(1, 1)),
				new fgl_triangle(new fgl_vec3(0, 0, 0), new fgl_vec3(1, 1, 0), new fgl_vec3(1, 0, 0),
								 new fgl_vec2(0, 0), new fgl_vec2(1, 1), new fgl_vec2(1, 0))
			};
			fgl_TriangleBufferData(TriangleBuffer, Triangles, TriangleCount = Triangles.Length);//*/

			// Load texture
			byte[] ImageData = File.ReadAllBytes("models\\diablo3_pose\\diablo3_pose_diffuse.png");
			fgl_BindTexture(TextureBuffer = fgl_CreateFramebufferFromImage(ImageData, ImageData.Length), 0);
		}

		static void Render() {
			fgl_ClearFramebuffer(ColorBuffer, fgl_color.Black);
			fgl_ClearFramebuffer(DepthBuffer, fgl_color.DepthZero);
			fgl_SetModelMatrix(Matrix4x4.CreateRotationY((float)SWatch.ElapsedMilliseconds / 5000));
			fgl_DrawTriangleBuffer(TriangleBuffer);
		}

		static void UpdateTexture(Texture Tex, IntPtr Framebuffer) {
			int Len;
			IntPtr FramebufferDataPtr = fgl_GetFramebufferData(Framebuffer, out Len);

			if (FramebufferData == null || FramebufferData.Length != Len)
				FramebufferData = new byte[Len];

			Marshal.Copy(FramebufferDataPtr, FramebufferData, 0, Len);
			Tex.Update(FramebufferData);
		}

		public static void Main(string[] Args) {
			VideoMode VMode = new VideoMode((uint)W, (uint)H);
			RenderWindow RWind = new RenderWindow(VMode, "FishGUI", Styles.Close);
			RWind.SetVerticalSyncEnabled(false);
			RWind.SetFramerateLimit(0);
			RWind.Closed += (S, E) => RWind.Close();

			RWind.KeyReleased += (S, E) => {
				switch (E.Code) {
					case Keyboard.Key.Escape:
						RWind.Close();
						break;
				}
			};

			Font DrawFont = new Font("C:\\Windows\\Fonts\\Consola.ttf");

			Text InfoText = new Text("Hello World!", DrawFont, 12);
			InfoText.Position = new Vector2f(1, 1);

			Text InfoText2 = new Text("[put text here]", DrawFont, 12);
			InfoText2.Position = new Vector2f(1, 50);

			Texture Tex = new Texture(VMode.Width, VMode.Height);
			Sprite TexSprite = new Sprite(Tex);
			TexSprite.Scale = new Vector2f(1, -1);
			TexSprite.Position = new Vector2f(0, H);

			Stopwatch SWatch = new Stopwatch();
			float TPS = 1.0f / Stopwatch.Frequency;
			float FrameTime = 0;

			Init();

			while (RWind.IsOpen) {
				RWind.DispatchEvents();
				RWind.Clear(Color.Black);

				//SWatch.Restart();
				Render();
				//SWatch.Stop();

				//Tex.Update(FishGL.ColorBuffer.Data);
				UpdateTexture(Tex, ColorBuffer);

				FrameTime = SWatch.ElapsedTicks * TPS;
				SWatch.Restart();

				InfoText.DisplayedString = string.Format("{0:0.0000} ms; {1} FPS\n{2} tris",
					FrameTime * 1000.0f, 1.0f / FrameTime, TriangleCount);
				RWind.Draw(TexSprite);
				RWind.Draw(InfoText);
				RWind.Draw(InfoText2);
				RWind.Display();
			}
		}
	}
}