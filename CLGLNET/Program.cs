//using OpenTK;
//using OpenTK.Graphics.OpenGL4;
//using OpenTK.Compute.OpenCL;
//using System;
//using System.IO;
//using static OpenTK.Graphics.OpenGL.GL;
////using static OpenTK.Compute.OpenCL.CL;
//using System.Runtime.CompilerServices;
//using System.Runtime.InteropServices;
//using System;
//using System.IO;

using OpenTK;
using OpenTK.Graphics.OpenGL4;
using OpenTK.Compute.OpenCL;
using System;
using System.IO;


class Program
{
    static int vbo, nbo;
    static CLContext clContext;
    static CLCommandQueue queue;
    static CLKernel kernel;
    static CLBuffer aaBuf, bbBuf, clVbo, clNbo;

    static int N_X = 100;
    static int N_Y = 100;


    static void Main(string[] args)
    {
        // Inicjalizacja OpenGL
        InitOpenGL();

        // Inicjalizacja OpenCL
        InitOpenCL();

        // Przykładowe dane
        Punkt[] aa = new Punkt[N_X * N_Y];
        float dt = 0.01f;
        float w = -2.0f;

        // Uruchomienie kernela
        RunKernel(aa, dt, w, N_X, N_Y);

        // Renderowanie
        Render();
    }

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
        unsafe
        {

            var platformIds = CL.GetPlatformIds(out var platforms);
            if (platformIds != CLResultCode.Success)
            {
                throw new Exception("Failed to get OpenCL platforms.");
            }

            var deviceIds = CL.GetDeviceIds(platforms[0], DeviceType.Gpu, out var devices);
            if (deviceIds != CLResultCode.Success)
            {
                throw new Exception("Failed to get OpenCL devices.");
            }

            clContext = CL.CreateContext(IntPtr.Zero, devices, IntPtr.Zero, IntPtr.Zero, out var resultCode);
            if (resultCode != CLResultCode.Success)
            {
                throw new Exception("Failed to create OpenCL context.");
            }
            queue = CL.CreateCommandQueue(clContext, devices[0],CommandQueueProperties.None);

            string kernelSource = File.ReadAllText("kernel.cl");
            var program = CL.CreateProgramWithSource(clContext, kernelSource);
            CL.BuildProgram(program, devices, string.Empty);
            kernel = CL.CreateKernel(program, "obliczWspolrzedneINormalne");

            aaBuf = CL.CreateBuffer(clContext, MemFlags.ReadOnly, sizeof(Punkt) * N_X * N_Y);
            bbBuf = CL.CreateBuffer(clContext, MemFlags.ReadWrite, sizeof(Punkt) * N_X * N_Y);
            clVbo = CL.CreateFromGLBuffer(clContext, MemFlags.WriteOnly, vbo);
            clNbo = CL.CreateFromGLBuffer(clContext, MemFlags.WriteOnly, nbo);

        }
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
        CL.EnqueueAcquireGLObjects(queue, new[] { clVbo, clNbo });

        var globalWorkSize = new IntPtr[] { (IntPtr)N_X, (IntPtr)N_Y };
        CL.EnqueueNDRangeKernel(queue, kernel, 2, null, globalWorkSize, null);

        CL.EnqueueReleaseGLObjects(queue, new[] { clVbo, clNbo });
        CL.Finish(queue);
    }

    static void Render()
    {
        unsafe
        {
            GL.BindBuffer(BufferTarget.ArrayBuffer, vbo);
            GL.VertexAttribPointer(0, 3, VertexAttribPointerType.Float, false, sizeof(Punkt), IntPtr.Zero);
            GL.EnableVertexAttribArray(0);

            GL.BindBuffer(BufferTarget.ArrayBuffer, nbo);
            GL.VertexAttribPointer(1, 3, VertexAttribPointerType.Float, false, sizeof(Normal), IntPtr.Zero);
            GL.EnableVertexAttribArray(1);
        }
        // ... (rysowanie obiektów)
    }
}

[StructLayout(LayoutKind.Sequential)]
struct Punkt
{
    public float m, v, x;
}

[StructLayout(LayoutKind.Sequential)]
struct Normal
{
    public float nx, ny, nz;
}
