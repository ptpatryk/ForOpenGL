using OpenTK.Graphics.OpenGL;
using System.Runtime.InteropServices;
using OpenTK.Compute.OpenCL;
using OpenTK.Windowing.Common;
using OpenTK.Mathematics;
using OpenTK.Windowing.Desktop;
using static OpenTK.Graphics.OpenGL.GL;

namespace CLGLNET
{
    [StructLayout(LayoutKind.Sequential)]
    struct Punkt
    {
        public float m, v, x;
    }

    [StructLayout(LayoutKind.Sequential)]
    struct PunktNormal
    {
        public float x, y, z, nx, ny, nz;
    }

    public partial class WindowsWave : GameWindow
    {
        int nbo;
        CLCommandQueue queue;
        CLKernel kernel;
        CLKernel kernelTrujkatow;
        CLKernel kernelPrzygotujTrojkaty;
        CLBuffer aaBuf, bbBuf;
        CLBuffer clNbo;
        CLBuffer vertexBuffer;
        object _bufferLock = new object();
        private CLContext context;

        int tr = 2;

        int N_X = 100;
        int N_Y = 100;
        float dt = 0.01f;
        float w = -2.0f;
        //PunktNormal[] punktyINormalne;
        Punkt[] aa;
        float[] vertices;

        private int _vertexBufferObject;
        private int _vertexArrayObject;
        private int _shaderProgram;

        CLResultCode res;
        CLEvent clEvent;

        public WindowsWave(int width, int height, string title)
            : base(GameWindowSettings.Default, new NativeWindowSettings() { ClientSize = new OpenTK.Mathematics.Vector2i(width, height), Title = title })
        {
            aa = new Punkt[N_X * N_Y];

            for (int i = 0; i < aa.Length; i++)
            {
                aa[i].m = 1.0f;
            }

            vertices = new float[N_X * N_Y * 18*tr]; // Rozmiar bufora

            //TargetUpdateFrequency = 60.0;
        }

        //public WindowsWave(int width, int height, string title)
        //    : base(GameWindowSettings.Default, new NativeWindowSettings()
        //    {
        //        ClientSize = new OpenTK.Mathematics.Vector2i(width, height),
        //        Title = title,
        //        // Ustawienia kontekstu graficznego
        //        APIVersion = new Version(4, 1), // Wybierz wersję OpenGL - było 4,5
        //        Flags = ContextFlags.ForwardCompatible, // Ustaw flagi kontekstu
        //        Profile = ContextProfile.Core // Ustaw profil kontekstu
        //    })
        //{
        //    // Inicjalizacja danych
        //    aa = new Punkt[N_X * N_Y];
        //    for (int i = 0; i < aa.Length; i++)
        //    {
        //        aa[i].m = 1.0f;
        //    }
        //    vertices = new float[N_X * N_Y * 18]; // Rozmiar bufora
        //}



        private int _vertexShader;
        private int _fragmentShader;
        private float czas = 0.0f;

        protected override void OnLoad()
        {
            base.OnLoad();

            InitOpenGL();

            InitOpenCL();

            // Ustawienia światła
            GL.UseProgram(_shaderProgram);
            GL.Uniform3(GL.GetUniformLocation(_shaderProgram, "lightPos"), new Vector3(1.2f, 1.0f, 30.0f));
            GL.Uniform3(GL.GetUniformLocation(_shaderProgram, "viewPos"), new Vector3(0.0f, 0.0f, 3.0f));
            GL.Uniform3(GL.GetUniformLocation(_shaderProgram, "lightColor"), new Vector3(1.0f, 1.0f, 1.0f));
            GL.Uniform3(GL.GetUniformLocation(_shaderProgram, "objectColor"), new Vector3(1.0f, 0.5f, 0.31f));
        }

        protected override void OnUnload()
        {
            base.OnUnload();

            // Zwolnienie zasobów OpenGL
            GL.DeleteShader(_vertexShader);
            GL.DeleteShader(_fragmentShader);
            GL.DeleteProgram(_shaderProgram);
            GL.DeleteBuffer(_vertexBufferObject);
            GL.DeleteVertexArray(_vertexArrayObject);

            // Czyszczenie zasobów
            CL.ReleaseMemoryObject(aaBuf);
            CL.ReleaseMemoryObject(bbBuf);
            CL.ReleaseMemoryObject(clNbo);
            CL.ReleaseCommandQueue(queue);
            CL.ReleaseContext(context);
        }

        private void Renderuj()
        {
            //GL.BufferData(BufferTarget.ArrayBuffer, vertices.Length * sizeof(float), vertices, BufferUsageHint.DynamicDraw);
        }

        protected override void OnRenderFrame(FrameEventArgs e)
        {
            /*
            base.OnRenderFrame(e);

            GL.Clear(ClearBufferMask.ColorBufferBit);
            //CheckGLError("GL.Clear");

            GL.UseProgram(_shaderProgram);
            //CheckGLError("GL.UseProgram");

            GL.BindVertexArray(_vertexArrayObject);
            //CheckGLError("GL.BindVertexArray");

            //GL.LoadIdentity();
            //CheckGLError("GL.LoadIdentity");

            // Rotate the image
            Matrix4 rotationMatrixZ = Matrix4.CreateRotationZ(MathHelper.DegreesToRadians(45.0f)); // Rotate 10 degrees around X axis
            Matrix4 rotationMatrixX = Matrix4.CreateRotationX(MathHelper.DegreesToRadians(30.0f)); // Rotate 30 degrees around Y axis
            Matrix4 rotationMatrix = rotationMatrixZ * rotationMatrixX; // Combine rotations

            Matrix4 projection = Matrix4.CreateOrthographic(150.0f, 150.0f, -50.0f, 50.0f);
            GL.UniformMatrix4(GL.GetUniformLocation(_shaderProgram, "projection"), false, ref projection);
            //CheckGLError("GL.UniformMatrix4 (projection)");

            int location = GL.GetUniformLocation(_shaderProgram, "rotationMatrix");
            GL.UniformMatrix4(location, false, ref rotationMatrix);
            //CheckGLError("GL.UniformMatrix4 (rotationMatrix)");

            GL.DrawArrays(PrimitiveType.Triangles, 0, N_X * N_Y * 2);
            //CheckGLError("GL.DrawArrays");

            SwapBuffers();
            //CheckGLError("SwapBuffers");
            */
            //nowe:
            base.OnRenderFrame(e);

            GL.Clear(ClearBufferMask.ColorBufferBit | ClearBufferMask.DepthBufferBit);

            GL.UseProgram(_shaderProgram);

            // Ustawienia macierzy modelu, widoku i projekcji
            Matrix4 model = Matrix4.Identity;
            Matrix4 view = Matrix4.LookAt(new Vector3(0.0f, 0.0f, 3.0f), Vector3.Zero, Vector3.UnitY);

            #region wykombinowane
            // Rotate the image
            Matrix4 rotationMatrixZ = Matrix4.CreateRotationZ(MathHelper.DegreesToRadians(45.0f)); // Rotate 10 degrees around X axis
            Matrix4 rotationMatrixX = Matrix4.CreateRotationX(MathHelper.DegreesToRadians(30.0f)); // Rotate 30 degrees around Y axis
            Matrix4 rotationMatrix = rotationMatrixZ * rotationMatrixX; // Combine rotations

            Matrix4 projection = rotationMatrix*Matrix4.CreateOrthographic(150.0f, 150.0f, -50.0f, 50.0f);

            #endregion

            //Matrix4 projection = Matrix4.CreatePerspectiveFieldOfView(MathHelper.DegreesToRadians(45.0f), Size.X / (float)Size.Y, 0.1f, 100.0f);

            GL.UniformMatrix4(GL.GetUniformLocation(_shaderProgram, "model"), false, ref model);
            GL.UniformMatrix4(GL.GetUniformLocation(_shaderProgram, "view"), false, ref view);
            GL.UniformMatrix4(GL.GetUniformLocation(_shaderProgram, "projection"), false, ref projection);

            GL.BindVertexArray(_vertexArrayObject);
            GL.DrawArrays(PrimitiveType.Triangles, 0, N_X * N_Y * 2);

            SwapBuffers();
        }

        void InitOpenGL()
        {
            #region przygotawanie schaderów

            string vertexShaderSource = File.ReadAllText("vertex_shader.glsl");

            string fragmentShaderSource = File.ReadAllText("fragment_shader.glsl");

            _vertexShader = GL.CreateShader(ShaderType.VertexShader);
            GL.ShaderSource(_vertexShader, vertexShaderSource);
            GL.CompileShader(_vertexShader);

            _fragmentShader = GL.CreateShader(ShaderType.FragmentShader);
            GL.ShaderSource(_fragmentShader, fragmentShaderSource);
            GL.CompileShader(_fragmentShader);

            _shaderProgram = GL.CreateProgram();
            GL.AttachShader(_shaderProgram, _vertexShader);
            GL.AttachShader(_shaderProgram, _fragmentShader);
            GL.LinkProgram(_shaderProgram);

            #endregion

            // Utwórz bufor współdzielony między OpenCL i OpenGL
            GL.GenBuffers(1, out nbo);
            CheckGLError("Tworzenie Bufora");
            GL.BindBuffer(BufferTarget.ArrayBuffer, nbo);
            GL.BufferData(BufferTarget.ArrayBuffer, sizeof(float) * N_X * N_Y * 18*tr, IntPtr.Zero, BufferUsageHint.DynamicDraw);

            _vertexArrayObject = GL.GenVertexArray();
            GL.BindVertexArray(_vertexArrayObject);

            GL.VertexAttribPointer(0, 3, VertexAttribPointerType.Float, false, 6 * sizeof(float), 0);
            GL.EnableVertexAttribArray(0);

            GL.VertexAttribPointer(1, 3, VertexAttribPointerType.Float, false, 6 * sizeof(float), 3 * sizeof(float));
            GL.EnableVertexAttribArray(1);
            CheckGLError("Ostatnia linia");
        }

        void InitOpenCL()
        {
            // Pobierz aktualny kontekst OpenGL
            IntPtr glContext = wglGetCurrentContext();
            IntPtr glDC = wglGetCurrentDC();

            // Platforma i urządzenie
            res = CL.GetPlatformIds(out CLPlatform[] platformIds);

            WyswietlPlatformy(platformIds);

            CLPlatform myPlatform = platformIds[0];

            res = CL.GetDeviceIds(myPlatform, OpenTK.Compute.OpenCL.DeviceType.Gpu, out CLDevice[] deviceIds);
            CheckResult(res);
            var device = deviceIds[0];

            IntPtr platform = myPlatform.Handle;
            IntPtr[] properties = new[] {
                   (IntPtr)0x1084, platform,
                   (IntPtr)0x2008, glContext, // GL_CONTEXT_KHR
                   (IntPtr)0x200A, glDC,   // WGL_HDC_KHR
                   IntPtr.Zero
                };

            // Kontekst i kolejka
            context = CL.CreateContext(properties, deviceIds, IntPtr.Zero, IntPtr.Zero, out res);
            CheckResult(res);

            // Replace the line causing the error with the following line
            queue = CL.CreateCommandQueueWithProperties(context, device, IntPtr.Zero, out res);
            CheckResult(res);

            // Program i kernel
            string programSource = File.ReadAllText("kernel.cl");
            var program = CL.CreateProgramWithSource(context, programSource, out res);
            CheckResult(res);

            res = CL.BuildProgram(program, 1, new[] { device }, string.Empty, IntPtr.Zero, IntPtr.Zero);
            CheckResult(res);

            kernel = CL.CreateKernel(program, "obliczWspolrzedne", out res);
            CheckResult(res);

            kernelTrujkatow = CL.CreateKernel(program, "obliczNormalne", out res);
            CheckResult(res);

            kernelPrzygotujTrojkaty = CL.CreateKernel(program, "przygotujTrojkaty", out res);
            CheckResult(res);

            // Dane wejściowe
            Punkt[] b = new Punkt[N_X * N_Y];  //może w miejscu b dać aa
            PunktNormal[] c = new PunktNormal[N_X * N_Y];
            float[] d = new float[N_X * N_Y * 18*tr * 4];

            // Bufory
            aaBuf = CL.CreateBuffer<Punkt>(context, MemoryFlags.ReadWrite | MemoryFlags.CopyHostPtr, aa, out res);
            CheckResult(res);

            bbBuf = CL.CreateBuffer<Punkt>(context, MemoryFlags.ReadWrite | MemoryFlags.CopyHostPtr, b, out res);
            CheckResult(res);

            clNbo = CL.CreateBuffer<PunktNormal>(context, MemoryFlags.ReadWrite | MemoryFlags.CopyHostPtr, c, out res);
            CheckResult(res);

            //vertexBuffer = CL.CreateBuffer<float>(context, MemoryFlags.ReadWrite | MemoryFlags.CopyHostPtr, d, out res);
            //CheckResult(res);
            // Utwórz bufor współdzielony między OpenCL i OpenGL
            vertexBuffer = CreateFromGLBuffer(context, MemoryFlags.ReadWrite, nbo, out res);
            CheckResult(res);
        }

        bool kt2 = true;

        protected override void OnUpdateFrame(FrameEventArgs e)
        {
            kt2 = RunKernel(kt2);
            Renderuj();
            czas += dt;
        }

        bool RunKernel(bool kt)
        {
            var b1 = kt ? aaBuf : bbBuf;
            var b2 = kt ? bbBuf : aaBuf;

            lock (_bufferLock)
            {
                // Ustawienie argumentów kernela
                CL.SetKernelArg(kernel, 0, b1);
                CL.SetKernelArg(kernel, 1, b2);
                CL.SetKernelArg(kernel, 2, dt);
                CL.SetKernelArg(kernel, 3, w);
                CL.SetKernelArg(kernel, 4, N_X);
                CL.SetKernelArg(kernel, 5, N_Y);

                //do wzbudzenia fal
                CL.SetKernelArg(kernel, 6, (float)Math.Sin(czas));
                CL.SetKernelArg(kernel, 7, czas);

                CL.SetKernelArg(kernelTrujkatow, 0, b2);
                CL.SetKernelArg(kernelTrujkatow, 1, clNbo);
                CL.SetKernelArg(kernelTrujkatow, 2, N_X);
                CL.SetKernelArg(kernelTrujkatow, 3, N_Y);

                CL.SetKernelArg(kernelPrzygotujTrojkaty, 0, clNbo);
                CL.SetKernelArg(kernelPrzygotujTrojkaty, 1, vertexBuffer);
                CL.SetKernelArg(kernelPrzygotujTrojkaty, 2, N_X);
                CL.SetKernelArg(kernelPrzygotujTrojkaty, 3, N_Y);

                //GL.Finish();

                // Akwizycja bufora współdzielonego
                res = EnqueueAcquireGLObjects(queue, 1, new[] { vertexBuffer }, 0, null, out _);
                //CheckResult(res);

                // Uruchomienie kernela
                var globalWorkSize = new nuint[] { (nuint)N_X, (nuint)N_Y };
                uint liczbaWymiarow = 2;
                res = CL.EnqueueNDRangeKernel(queue, kernel, liczbaWymiarow, null, globalWorkSize, null, 0, null, out _);
                //CheckResult(res);

                res = CL.EnqueueNDRangeKernel(queue, kernelTrujkatow, liczbaWymiarow, null, globalWorkSize, null, 0, null, out _);
                //CheckResult(res);

                res = CL.EnqueueNDRangeKernel(queue, kernelPrzygotujTrojkaty, liczbaWymiarow, null, globalWorkSize, null, 0, null, out _);
                //CheckResult(res);

                //CL.EnqueueReadBuffer(queue, vertexBuffer, true, UIntPtr.Zero, vertices, null, out CLEvent _);

                res = EnqueueReleaseGLObjects(queue, 1, new[] { vertexBuffer }, 0, null, out _);
                //CheckResult(res);

                CL.Finish(queue);
            }
            return !kt;
        }

    }
}
