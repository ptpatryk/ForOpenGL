using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using OpenTK;
//using OpenTK.Graphics.OpenGL4;
using OpenTK.Graphics.GL;

using OpenTK.Graphics;
using OpenTK.Graphics.OpenGL;
using OpenTK.Input;

using System;
using System.IO;
using System.Runtime.InteropServices;
using static OpenTK.Graphics.OpenGL.GL;
//using OpenTK.Windowing.Desktop;
using OpenTK.Compute.OpenCL;
using System;
using System.IO;
using OpenTK.Windowing.Common;
using OpenTK.Mathematics;
using System.Threading.Tasks.Dataflow;
using OpenTK.Graphics.Wgl;
using static System.Runtime.InteropServices.JavaScript.JSType;
//using OpenGLTetris.Blocks;
using OpenTK;
using OpenTK.Graphics;
using OpenTK.Graphics.OpenGL;
using OpenTK.Input;
using OpenTK.Windowing.Desktop;

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

    public class WindowsWave : GameWindow
    {
        int vbo, nbo;
        //ComputeContext clContext;
        CLCommandQueue queue;
        CLKernel kernel;
        CLKernel kernelTrujkatow;
        CLKernel kernelPrzygotujTrojkaty;
        CLBuffer aaBuf, bbBuf;
        CLBuffer clNbo;
        CLBuffer vertexBuffer;
        object _bufferLock = new object();
        private CLContext context;

        int N_X = 100;
        int N_Y = 100;
        float dt = 0.01f;
        float w = -2.0f;
        PunktNormal[] punktyINormalne;
        Punkt[] aa;

        private int _vertexBufferObject;
        private int _vertexArrayObject;
        private int _shaderProgram;


        CLResultCode res;
        CLEvent clEvent;

        public WindowsWave(int width, int height, string title)
            : base(GameWindowSettings.Default, new NativeWindowSettings() { ClientSize = new OpenTK.Mathematics.Vector2i(width, height), Title = title })
        {
            punktyINormalne = new PunktNormal[N_X * N_Y];

            aa = new Punkt[N_X * N_Y];

            for (int i = 0; i < aa.Length; i++)
            {
                aa[i].m = 1.0f;
            }

            //TargetUpdateFrequency = 60.0;

        }

        private int _vertexShader;
        private int _fragmentShader;
        private float czas = 0.0f;

        protected override void OnLoad()
        {
            base.OnLoad();

            InitOpenGL();

            InitOpenCL();
          
            RunKernel(true, czas);

            //Akcja();
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

  
        private void Renderuj(float[] vertices)
        {
            lock (_bufferLock)
            {
                GL.BindBuffer(BufferTarget.ArrayBuffer, _vertexBufferObject);
                GL.BufferData(BufferTarget.ArrayBuffer, vertices.Length * sizeof(float), vertices, BufferUsageHint.DynamicDraw);

                GL.BindVertexArray(_vertexArrayObject);
                GL.VertexAttribPointer(0, 3, VertexAttribPointerType.Float, false, 6 * sizeof(float), 0);
                GL.EnableVertexAttribArray(0);

                GL.VertexAttribPointer(1, 3, VertexAttribPointerType.Float, false, 6 * sizeof(float), 3 * sizeof(float));
                GL.EnableVertexAttribArray(1);
                         
                //GL.Flush();
            }
        }

        protected override void OnRenderFrame(FrameEventArgs e)
        {
            base.OnRenderFrame(e);

            GL.Clear(ClearBufferMask.ColorBufferBit);
            CheckGLError("GL.Clear");


            GL.UseProgram(_shaderProgram);
            CheckGLError("GL.UseProgram");

            GL.BindVertexArray(_vertexArrayObject);
            CheckGLError("GL.BindVertexArray");

            //GL.LoadIdentity();
            //CheckGLError("GL.LoadIdentity");

            // Rotate the image
            Matrix4 rotationMatrixZ = Matrix4.CreateRotationZ(MathHelper.DegreesToRadians(45.0f)); // Rotate 10 degrees around X axis
            Matrix4 rotationMatrixX = Matrix4.CreateRotationX(MathHelper.DegreesToRadians(30.0f)); // Rotate 30 degrees around Y axis
            Matrix4 rotationMatrix = rotationMatrixZ * rotationMatrixX; // Combine rotations

            Matrix4 projection = Matrix4.CreateOrthographic(150.0f, 150.0f, -50.0f, 50.0f);
            GL.UniformMatrix4(GL.GetUniformLocation(_shaderProgram, "projection"), false, ref projection);
            CheckGLError("GL.UniformMatrix4 (projection)");


            int location = GL.GetUniformLocation(_shaderProgram, "rotationMatrix");
            GL.UniformMatrix4(location, false, ref rotationMatrix);
            CheckGLError("GL.UniformMatrix4 (rotationMatrix)");

            GL.DrawArrays(PrimitiveType.Triangles, 0, N_X * N_Y * 2);
            CheckGLError("GL.DrawArrays");

            SwapBuffers();
            CheckGLError("SwapBuffers");
        }

        unsafe void InitOpenGL()
        {
            #region tu mam problem miało być to do dzielenia zasobów między OpenCL i OpenGL

            GL.GenBuffers(1, out nbo);
            GL.BindBuffer(BufferTarget.ArrayBuffer, nbo);
            //GL.BufferData(BufferTarget.ArrayBuffer, sizeof(PunktNormal) * N_X * N_Y, IntPtr.Zero, BufferUsageHint.DynamicDraw);
            GL.BufferData(BufferTarget.ArrayBuffer, sizeof(float) * N_X * N_Y * 18, IntPtr.Zero, BufferUsageHint.DynamicDraw);

            #endregion

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

            // Initialize vertex buffer object and vertex array object
            _vertexBufferObject = GL.GenBuffer();
            GL.BindBuffer(BufferTarget.ArrayBuffer, _vertexBufferObject);
            GL.BufferData(BufferTarget.ArrayBuffer, sizeof(float) * N_X * N_Y * 18, IntPtr.Zero, BufferUsageHint.DynamicDraw);

            _vertexArrayObject = GL.GenVertexArray();
            GL.BindVertexArray(_vertexArrayObject);

            GL.VertexAttribPointer(0, 3, VertexAttribPointerType.Float, false, 6 * sizeof(float), 0);
            GL.EnableVertexAttribArray(0);

            GL.VertexAttribPointer(1, 3, VertexAttribPointerType.Float, false, 6 * sizeof(float), 3 * sizeof(float));
            GL.EnableVertexAttribArray(1);

        }

        unsafe void InitOpenCL()
        {
            // Platforma i urządzenie
            res = CL.GetPlatformIds(out CLPlatform[] platformIds);
           
            res = CL.GetDeviceIds(platformIds[0], DeviceType.Gpu, out CLDevice[] deviceIds);
            CheckResult(res);
            var device = deviceIds[0];

            // Kontekst i kolejka
            context = CL.CreateContext(IntPtr.Zero, 1, new[] { device }, IntPtr.Zero, IntPtr.Zero, out res);
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
            Punkt[] a = new Punkt[N_X * N_Y];
            Punkt[] b = new Punkt[N_X * N_Y];
            PunktNormal[] c = new PunktNormal[N_X * N_Y];
            float[] d = new float[N_X * N_Y * 18 * 4];

            // Bufory
            aaBuf = CL.CreateBuffer<Punkt>(context, MemoryFlags.ReadWrite | MemoryFlags.CopyHostPtr, aa, out res);
            CheckResult(res);

            //aaBuf = CL.CreateBuffer(context, MemoryFlags.ReadWrite | MemoryFlags.CopyHostPtr, (uint)(a.Length * sizeof(Punkt)), (nint)GCHandle.Alloc(a, GCHandleType.Pinned).AddrOfPinnedObject(), out _);
            bbBuf = CL.CreateBuffer<Punkt>(context, MemoryFlags.ReadWrite | MemoryFlags.CopyHostPtr, b, out res);
            CheckResult(res);

            clNbo = CL.CreateBuffer<PunktNormal>(context, MemoryFlags.ReadWrite | MemoryFlags.CopyHostPtr, c, out res);
            CheckResult(res);

            vertexBuffer = CL.CreateBuffer<float>(context, MemoryFlags.ReadWrite | MemoryFlags.CopyHostPtr, d, out res);
            CheckResult(res);
        }


        private readonly object _syncLock = new object();

        //async Task Akcja()
        //{
        //    bool kt = false;
        //    while (true)
        //    {
        //        lock (_syncLock)
        //        {
        //            kt = RunKernel(kt, czas);
        //            czas += dt;

        //        }
        //        Thread.Sleep(1000);
        //        //await Task.Delay(1000);
        //    }
        //}

        bool kt2 = false;

        protected override void OnUpdateFrame(FrameEventArgs e)
        {
            lock (_syncLock)
            {
                kt2 = RunKernel(kt2, czas);
                czas += dt;

            }

        }


        unsafe bool RunKernel(bool kt, float czas)
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

                /*
                var queue = new ComputeCommandQueue(context, context.Devices[0], ComputeCommandQueueFlags.None);
                queue.AcquireGLObjects(new[] { clBuffer }, null);
                // Operacje OpenCL na buforze
                queue.ReleaseGLObjects(new[] { clBuffer }, null);
                queue.Finish();
                */
                //queue.AcquireGLObjects(new[] { vertexBuffer }, null);              


                // Uruchomienie kernela
                var globalWorkSize = new nuint[] { (nuint)N_X, (nuint)N_Y };
                uint liczbaWymiarow = 2;
                res = CL.EnqueueNDRangeKernel(queue, kernel, liczbaWymiarow, null, globalWorkSize, null, 0, null, out _);
                CheckResult(res);

                res = CL.EnqueueNDRangeKernel(queue, kernelTrujkatow, liczbaWymiarow, null, globalWorkSize, null, 0, null, out _);
                CheckResult(res);

                res = CL.EnqueueNDRangeKernel(queue, kernelPrzygotujTrojkaty, liczbaWymiarow, null, globalWorkSize, null, 0, null, out _);
                CheckResult(res);


                // Zakładamy, że vertexBuffer jest już zainicjalizowany i wypełniony danymi
                var vertices = new float[N_X * N_Y * 18]; // Rozmiar bufora
                //var punktyINormalne = new PunktNormal[N_X * N_Y];
                //var punkty = new Punkt[N_X * N_Y];  

                // Odczyt danych z bufora
                CL.EnqueueReadBuffer(queue, vertexBuffer, true, (nuint)0, (nuint)(vertices.Length * sizeof(float)), (nint)GCHandle.Alloc(vertices, GCHandleType.Pinned).AddrOfPinnedObject(), 0, null, out _);
                //CL.EnqueueReadBuffer(queue, clNbo, true, (nuint)0, (nuint)(punktyINormalne.Length * sizeof(PunktNormal)), (nint)GCHandle.Alloc(punktyINormalne, GCHandleType.Pinned).AddrOfPinnedObject(), 0, null, out _);
                //CL.EnqueueReadBuffer(queue, b1, true, (nuint)0, (nuint)(punkty.Length * sizeof(Punkt)), (nint)GCHandle.Alloc(punkty, GCHandleType.Pinned).AddrOfPinnedObject(), 0, null, out _);

                // Teraz tablica vertices zawiera dane z bufora vertexBuffer
                // Możesz je przetwarzać według potrzeb

                CL.Finish(queue);
                Renderuj(vertices);
            }
            return !kt;
        }

        private void CheckGLError(string location)
        {
            ErrorCode error = GL.GetError();
            if (error != ErrorCode.NoError)
            {
                throw new Exception($"OpenGL error at {location}: {error}");
            }
        }

        private static void CheckResult(CLResultCode res)
        {
            if (res != CLResultCode.Success)
            {
                //ViewError(res.ToString(), res.ToString());
                throw new Exception("Error");
            }
        }
    }
}
