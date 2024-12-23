//using OpenTK.Mathematics;
//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;
//using System.Threading.Tasks;

//using OpenTK.Graphics.OpenGL4;
//using Cloo;
using OpenTK;
using OpenTK.Graphics.OpenGL4;
using Cloo;
using System;
using System.IO;
using System.Runtime.InteropServices;
using static OpenTK.Graphics.OpenGL.GL;
using OpenTK.Windowing.Desktop;
//using OpenTK;
//using OpenTK.Graphics.OpenGL4;
using OpenTK.Compute.OpenCL;
using System;
using System.IO;
using System.Runtime.InteropServices;
using OpenTK.Windowing.Common;


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
        //int vbo, nbo;
        //CLContext clContext;
        //CLCommandQueue queue;
        //CLKernel kernel;
        //CLBuffer aaBuf, bbBuf, clNbo;

        int vbo, nbo;
        ComputeContext clContext;
        ComputeCommandQueue queue;
        ComputeKernel kernel;
        ComputeKernel kernelTrujkatow;
        ComputeBuffer<Punkt> aaBuf, bbBuf;
        ComputeBuffer<PunktNormal> clNbo;

        int N_X = 100;
        int N_Y = 100;

        private int _vertexBufferObject;
        private int _vertexArrayObject;
        private int _shaderProgram;

        public WindowsWave(int width, int height, string title)
            : base(GameWindowSettings.Default, new NativeWindowSettings() { Size = new OpenTK.Mathematics.Vector2i(width, height), Title = title })
        {
            
        }





        protected override void OnLoad()
        {
            base.OnLoad();

            
            // Inicjalizacja OpenGL
            InitOpenGL();

            // Inicjalizacja OpenCL
            InitOpenCL();

            // Przykładowe dane
            Punkt[] aa = new Punkt[N_X * N_Y];

            for (int i = 0; i < aa.Length; i++)
            {
                aa[i].m = 1.0f;
            }


            float dt = 0.01f;
            float w = -2.0f;
            //int N_X = 100;
            //int N_Y = 100;

            // Uruchomienie kernela
            RunKernel(aa, dt, w, N_X, N_Y);


            #region rysowania statycznie trójkątów (metoda 2)

//            float[] vertices = {
//    // Trójkąt 1
//    -0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,
//   -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,
//    0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,
//    // Trójkąt 2
//    -1.0f, 1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
//    1.0f,  -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
//    1.0f, 1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
//};

//            _vertexBufferObject = GL.GenBuffer();
//            GL.BindBuffer(BufferTarget.ArrayBuffer, _vertexBufferObject);
//            GL.BufferData(BufferTarget.ArrayBuffer, vertices.Length * sizeof(float), vertices, BufferUsageHint.StaticDraw);

//            _vertexArrayObject = GL.GenVertexArray();
//            GL.BindVertexArray(_vertexArrayObject);

//            GL.VertexAttribPointer(0, 3, VertexAttribPointerType.Float, false, 6 * sizeof(float), 0);
//            GL.EnableVertexAttribArray(0);

//            GL.VertexAttribPointer(1, 3, VertexAttribPointerType.Float, false, 6 * sizeof(float), 3 * sizeof(float));
//            GL.EnableVertexAttribArray(1);

//            string vertexShaderSource = @"
//        #version 330 core
//        layout(location = 0) in vec3 aPosition;
//        layout(location = 1) in vec3 aNormal;
//        void main()
//        {
//            gl_Position = vec4(aPosition, 1.0);
//        }";

//            string fragmentShaderSource = @"
//        #version 330 core
//        out vec4 FragColor;
//        void main()
//        {
//            FragColor = vec4(1.0, 0.5, 0.2, 1.0);
//        }";

//            int vertexShader = GL.CreateShader(ShaderType.VertexShader);
//            GL.ShaderSource(vertexShader, vertexShaderSource);
//            GL.CompileShader(vertexShader);

//            int fragmentShader = GL.CreateShader(ShaderType.FragmentShader);
//            GL.ShaderSource(fragmentShader, fragmentShaderSource);
//            GL.CompileShader(fragmentShader);

//            _shaderProgram = GL.CreateProgram();
//            GL.AttachShader(_shaderProgram, vertexShader);
//            GL.AttachShader(_shaderProgram, fragmentShader);
//            GL.LinkProgram(_shaderProgram);

//            GL.DeleteShader(vertexShader);
//            GL.DeleteShader(fragmentShader);

            #endregion
        }


        protected override void OnRenderFrame(FrameEventArgs e)
        {
            base.OnRenderFrame(e);

            #region rysowanie samych ze stałej tablicy trójkątów

            //GL.Clear(ClearBufferMask.ColorBufferBit);

            //GL.UseProgram(_shaderProgram);
            //GL.BindVertexArray(_vertexArrayObject);
            //GL.DrawArrays(PrimitiveType.Triangles, 0, 6);//rysuje 6 wierzchołków (2 trójkąty)

            #endregion

            #region rysowania z obliczanej tablicy trójkątów

            GL.Clear(ClearBufferMask.ColorBufferBit | ClearBufferMask.DepthBufferBit);

            Render();


            #endregion


            SwapBuffers();






        }

        unsafe void InitOpenGL()
        {
            //GL.GenBuffers(1, out vbo);
            //GL.BindBuffer(BufferTarget.ArrayBuffer, vbo);
            //GL.BufferData(BufferTarget.ArrayBuffer, sizeof(Punkt) * N_X * N_Y, IntPtr.Zero, BufferUsageHint.DynamicDraw);

            GL.GenBuffers(1, out nbo);
            GL.BindBuffer(BufferTarget.ArrayBuffer, nbo);
            GL.BufferData(BufferTarget.ArrayBuffer, sizeof(PunktNormal) * N_X * N_Y, IntPtr.Zero, BufferUsageHint.DynamicDraw);
        }

        unsafe void InitOpenCL()
        {
            /*
            var platform = ComputePlatform.Platforms[0];
            var devices = platform.Devices;
            clContext = new ComputeContext(devices, new ComputeContextPropertyList(platform), null, IntPtr.Zero);
            queue = new ComputeCommandQueue(clContext, devices[0], ComputeCommandQueueFlags.None);

            string kernelSource = File.ReadAllText("kernel.cl");
            var program = new ComputeProgram(clContext, kernelSource);
            program.Build(devices, null, null, IntPtr.Zero);
            kernel = program.CreateKernel("obliczWspolrzedneINormalne");

            aaBuf = new ComputeBuffer<Punkt>(clContext, ComputeMemoryFlags.ReadOnly | ComputeMemoryFlags.CopyHostPointer, new Punkt[N_X * N_Y]);
            bbBuf = new ComputeBuffer<Punkt>(clContext, ComputeMemoryFlags.ReadWrite, N_X * N_Y);
            clNbo = new ComputeBuffer<Normal>(clContext, ComputeMemoryFlags.WriteOnly, N_X * N_Y);
            */
            var platform = ComputePlatform.Platforms[0];
            var devices = platform.Devices;

            //// Get the current OpenGL context handle
            //IntPtr glContextHandle = OpenTK.Graphics.OpenGL.GL.GetCurrentContext();

            //// Create the OpenCL context properties with OpenGL context
            //var properties = new ComputeContextPropertyList(platform);
            //properties.Add(new ComputeContextProperty(ComputeContextPropertyName.CL_GL_CONTEXT_KHR, glContextHandle));
            //properties.Add(new ComputeContextProperty(ComputeContextPropertyName.CL_WGL_HDC_KHR, OpenTK.Platform.Windows.API.GetCurrentDC()));


            clContext = new ComputeContext(devices, new ComputeContextPropertyList(platform), null, IntPtr.Zero);
            queue = new ComputeCommandQueue(clContext, devices[0], ComputeCommandQueueFlags.None);

            string kernelSource = File.ReadAllText("kernel.cl");
            var program = new ComputeProgram(clContext, kernelSource);
            program.Build(devices, null, null, IntPtr.Zero);
            kernel = program.CreateKernel("obliczWspolrzedne");
            kernelTrujkatow = program.CreateKernel("obliczNormalne");

            aaBuf = new ComputeBuffer<Punkt>(clContext, ComputeMemoryFlags.ReadOnly | ComputeMemoryFlags.CopyHostPointer, new Punkt[N_X * N_Y]);
            bbBuf = new ComputeBuffer<Punkt>(clContext, ComputeMemoryFlags.ReadWrite, N_X * N_Y);
            clNbo = new ComputeBuffer<PunktNormal>(clContext, ComputeMemoryFlags.WriteOnly, N_X * N_Y);
        }

        unsafe void RunKernel(Punkt[] aa, float dt, float w, int N_X, int N_Y)
        {
            queue.WriteToBuffer(aa, aaBuf, true, null);

            kernel.SetMemoryArgument(0, aaBuf);
            kernel.SetMemoryArgument(1, bbBuf);
            kernel.SetValueArgument(2, dt);
            kernel.SetValueArgument(3, w);
            kernel.SetValueArgument(4, N_X);
            kernel.SetValueArgument(5, N_Y);

            kernelTrujkatow.SetMemoryArgument(0, bbBuf);
            kernelTrujkatow.SetMemoryArgument(1, clNbo);
            kernelTrujkatow.SetValueArgument(2, N_X);
            kernelTrujkatow.SetValueArgument(3, N_Y);


            GL.Finish();
            //queue.AcquireGLObjects(new[] { clNbo }, null);

            var globalWorkSize = new long[] { N_X, N_Y };

            queue.Execute(kernel, null, globalWorkSize, null, null);
            queue.Execute(kernelTrujkatow, null, globalWorkSize, null, null);

            PunktNormal[] normals = new PunktNormal[N_X * N_Y]; // Initialize the array
            queue.ReadFromBuffer(clNbo, ref normals, true, null);



            //queue.ReleaseGLObjects(new[] { clNbo }, null);
            queue.Finish();

            //Render();
        }

        unsafe void Render()
        {

            /*
            GL.BindBuffer(BufferTarget.ArrayBuffer, vbo);
            GL.VertexAttribPointer(0, 3, VertexAttribPointerType.Float, false, sizeof(Punkt), IntPtr.Zero);
            GL.EnableVertexAttribArray(0);

            GL.BindBuffer(BufferTarget.ArrayBuffer, nbo);
            GL.VertexAttribPointer(1, 3, VertexAttribPointerType.Float, false, sizeof(PunktNormal), IntPtr.Zero);
            GL.EnableVertexAttribArray(1);
            */

            
            GL.BindBuffer(BufferTarget.ArrayBuffer, nbo);
            GL.VertexAttribPointer(0, 6, VertexAttribPointerType.Float, false, sizeof(PunktNormal), IntPtr.Zero);
            GL.EnableVertexAttribArray(0);
            


            GL.DrawArrays(PrimitiveType.Triangles, 0, N_X * N_Y);
            // ... (rysowanie obiektów)



            //GL.Clear(ClearBufferMask.ColorBufferBit);

            //GL.UseProgram(_shaderProgram);
            //GL.BindVertexArray(_vertexArrayObject);
            //GL.DrawArrays(PrimitiveType.Triangles, 0, 6);//rysuje 6 wierzchołków (2 trójkąty)
        }

        /*

        static unsafe void InitOpenGL()
        {
            GL.GenBuffers(1, out vbo);
            GL.BindBuffer(BufferTarget.ArrayBuffer, vbo);
            GL.BufferData(BufferTarget.ArrayBuffer, sizeof(Punkt) * N_X * N_Y, IntPtr.Zero, BufferUsageHint.DynamicDraw);

            GL.GenBuffers(1, out nbo);
            GL.BindBuffer(BufferTarget.ArrayBuffer, nbo);
            GL.BufferData(BufferTarget.ArrayBuffer, sizeof(Normal) * N_X * N_Y, IntPtr.Zero, BufferUsageHint.DynamicDraw);
        }

        static void InitOpenCL()
        {
            var platforms = CL.GetPlatformIDs();
            var devices = CL.GetDeviceIDs(platforms[0], DeviceType.Gpu);
            clContext = CL.CreateContext(null, devices, null, IntPtr.Zero);
            queue = CL.CreateCommandQueue(clContext, devices[0], CommandQueueProperties.None);

            string kernelSource = File.ReadAllText("kernel.cl");
            var program = CL.CreateProgramWithSource(clContext, kernelSource);
            CL.BuildProgram(program, devices, string.Empty);
            kernel = CL.CreateKernel(program, "obliczWspolrzedneINormalne");

            aaBuf = CL.CreateBuffer(clContext, MemFlags.ReadOnly, sizeof(Punkt) * N_X * N_Y);
            bbBuf = CL.CreateBuffer(clContext, MemFlags.ReadWrite, sizeof(Punkt) * N_X * N_Y);
            clNbo = CL.CreateBuffer(clContext, MemFlags.WriteOnly, sizeof(Normal) * N_X * N_Y);
        }

        static unsafe void RunKernel(Punkt[] aa, float dt, float w, int N_X, int N_Y)
        {
            CL.EnqueueWriteBuffer(queue, aaBuf, true, 0, sizeof(Punkt) * N_X * N_Y, aa);

            CL.SetKernelArg(kernel, 0, aaBuf);
            CL.SetKernelArg(kernel, 1, bbBuf);
            CL.SetKernelArg(kernel, 2, clNbo);
            CL.SetKernelArg(kernel, 3, dt);
            CL.SetKernelArg(kernel, 4, w);
            CL.SetKernelArg(kernel, 5, N_X);
            CL.SetKernelArg(kernel, 6, N_Y);

            GL.Finish();
            CL.EnqueueAcquireGLObjects(queue, new[] { clNbo });

            var globalWorkSize = new IntPtr[] { (IntPtr)N_X, (IntPtr)N_Y };
            CL.EnqueueNDRangeKernel(queue, kernel, 2, null, globalWorkSize, null);

            CL.EnqueueReleaseGLObjects(queue, new[] { clNbo });
            CL.Finish(queue);
        }

        protected override void OnRenderFrame(FrameEventArgs e)
        {
            base.OnRenderFrame(e);

            GL.Clear(ClearBufferMask.ColorBufferBit | ClearBufferMask.DepthBufferBit);

            GL.BindBuffer(BufferTarget.ArrayBuffer, vbo);
            GL.VertexAttribPointer(0, 3, VertexAttribPointerType.Float, false, sizeof(Punkt), IntPtr.Zero);
            GL.EnableVertexAttribArray(0);

            GL.BindBuffer(BufferTarget.ArrayBuffer, nbo);
            GL.VertexAttribPointer(1, 3, VertexAttribPointerType.Float, false, sizeof(Normal), IntPtr.Zero);
            GL.EnableVertexAttribArray(1);

            // ... (rysowanie obiektów)

            SwapBuffers();
        }
        */
    }

}
