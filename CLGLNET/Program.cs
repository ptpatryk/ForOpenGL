using OpenTK;
using OpenTK.Graphics.OpenGL4;
using System;
using System.IO;
using System.Runtime.InteropServices;
using static OpenTK.Graphics.OpenGL.GL;
using OpenTK.Windowing.Desktop;
using CLGLNET;




class Program
{
    //static int vbo, nbo;
    //static ComputeContext clContext;
    //static ComputeCommandQueue queue;
    //static ComputeKernel kernel;
    //static ComputeBuffer<Punkt> aaBuf, bbBuf;
    //static ComputeBuffer<Normal> clNbo;


    //[DllImport("nvapi64.dll", EntryPoint = "NvAPI_Initialize", CallingConvention = CallingConvention.Cdecl)]
    //public static extern int NvAPI_Initialize();


    static void Main(string[] args)
    {
       // NvAPI_Initialize();
        using (var game = new WindowsWave(800, 600, "OpenGL with OpenCL"))
        {
            game.Run();
        }


        // Inicjalizacja OpenGL
        //InitOpenGL();

        /*
        var nativeWindowSettings = new NativeWindowSettings()
        {
            Size = new OpenTK.Mathematics.Vector2i(800, 600),
            Title = "OpenGL Example"
        };
        */
        /*
        using (var game = new GameWindow(800, 600, GraphicsMode.Default, "OpenGL Example"))
        {
            game.Load += (sender, e) =>
            {
                // Initialize OpenGL here
                InitOpenGL();
            };

            game.Run(60.0);
        }
        */
        //// Inicjalizacja OpenCL
        //InitOpenCL();

        //// Przykładowe dane
        //Punkt[] aa = new Punkt[N_X * N_Y];
        //float dt = 0.01f;
        //float w = -2.0f;

        //// Uruchomienie kernela
        //RunKernel(aa, dt, w, N_X, N_Y);

        //// Renderowanie
        //Render();
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
    }

    static unsafe void RunKernel(Punkt[] aa, float dt, float w, int N_X, int N_Y)
    {
        queue.WriteToBuffer(aa, aaBuf, true, null);

        kernel.SetMemoryArgument(0, aaBuf);
        kernel.SetMemoryArgument(1, bbBuf);
        kernel.SetMemoryArgument(2, clNbo);
        kernel.SetValueArgument(3, dt);
        kernel.SetValueArgument(4, w);
        kernel.SetValueArgument(5, N_X);
        kernel.SetValueArgument(6, N_Y);

        GL.Finish();
        queue.AcquireGLObjects(new[] { clNbo }, null);

        var globalWorkSize = new long[] { N_X, N_Y };
        queue.Execute(kernel, null, globalWorkSize, null, null);

        queue.ReleaseGLObjects(new[] { clNbo }, null);
        queue.Finish();
    }

    static unsafe void Render()
    {
        

            GL.BindBuffer(BufferTarget.ArrayBuffer, vbo);
            GL.VertexAttribPointer(0, 3, VertexAttribPointerType.Float, false, sizeof(Punkt), IntPtr.Zero);
            GL.EnableVertexAttribArray(0);

            GL.BindBuffer(BufferTarget.ArrayBuffer, nbo);
            GL.VertexAttribPointer(1, 3, VertexAttribPointerType.Float, false, sizeof(Normal), IntPtr.Zero);
            GL.EnableVertexAttribArray(1);
        // ... (rysowanie obiektów)
    }
    */
}

